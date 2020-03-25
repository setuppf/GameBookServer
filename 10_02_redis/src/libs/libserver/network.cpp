#include "network.h"
#include "connect_obj.h"
#include "packet.h"
#include "common.h"

#include "object_pool.h"
#include <iostream>
#include "component_help.h"
#include "object_pool_packet.h"

void Network::BackToPool()
{
    _sendMsgList.BackToPool();

    for(auto socket:_sockets)
    {
        auto pObj = _connects[socket];
        pObj->ComponentBackToPool();
        _connects[socket] = nullptr;
    }
    _sockets.clear();

#ifdef EPOLL
    ::close(_epfd);
    _epfd = -1;
#endif
}

void Network::SetSocketOpt(SOCKET socket)
{
    // 1.发送、接收timeout
    int netTimeout = 3000; // 1000 = 1秒
    setsockopt(socket, SOL_SOCKET, SO_SNDTIMEO, (SetsockOptType)&netTimeout, sizeof(netTimeout));
    setsockopt(socket, SOL_SOCKET, SO_RCVTIMEO, (SetsockOptType)&netTimeout, sizeof(netTimeout));

#if ENGINE_PLATFORM != PLATFORM_WIN32

    if (_networkType != NetworkType::HttpConnector && _networkType != NetworkType::HttpListen)
    {
        int keepAlive = 1;
        socklen_t optlen = sizeof(keepAlive);

        int keepIdle = 60 * 2;	// 在socket 没有交互后 多久 开始发送侦测包
        int keepInterval = 10;	// 多次发送侦测包之间的间隔
        int keepCount = 5;		// 侦测包个数

        setsockopt(socket, SOL_SOCKET, SO_KEEPALIVE, (SetsockOptType)&keepAlive, optlen);
        if (getsockopt(socket, SOL_SOCKET, SO_KEEPALIVE, &keepAlive, &optlen) < 0)
        {
            LOG_WARN("getsockopt SO_KEEPALIVE failed. err:" << _sock_err() << " socket:" << socket << " networktype:" << GetNetworkTypeName(_networkType));
        }

        setsockopt(socket, SOL_TCP, TCP_KEEPIDLE, (void*)&keepIdle, optlen);
        if (getsockopt(socket, SOL_TCP, TCP_KEEPIDLE, &keepIdle, &optlen) < 0)
        {
            LOG_WARN("getsockopt TCP_KEEPIDLE failed. err:" << _sock_err() << " socket:" << socket << " networktype:" << GetNetworkTypeName(_networkType));
        }

        setsockopt(socket, SOL_TCP, TCP_KEEPINTVL, (void*)&keepInterval, optlen);
        setsockopt(socket, SOL_TCP, TCP_KEEPCNT, (void*)&keepCount, optlen);
    }

#endif

    // 非阻塞
    _sock_nonblock(socket);
}

SOCKET Network::CreateSocket()
{
    _sock_init();
    SOCKET socket;

    if (_networkType == NetworkType::HttpListen || _networkType == NetworkType::HttpConnector)
        socket = ::socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
    else
        socket = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if (socket == INVALID_SOCKET)
    {
        std::cout << "::socket failed. err:" << _sock_err() << std::endl;
        return socket;
    }

    //LOG_DEBUG("create socket:" << socket << " networktype:" << GetNetworkTypeName(_networkType));

    SetSocketOpt(socket);
    return socket;
}

bool Network::CheckSocket(SOCKET socket)
{
    // 检查一下Socket是否有误
    int err = EBADF;
    socklen_t len = sizeof(err);
    if (::getsockopt(socket, SOL_SOCKET, SO_ERROR, (char*)(&err), &len) == 0)
    {
        // 获取成功，err也可能有错误的数据
        if (!NetworkHelp::IsError(err))
        {
            err = 0;
        }
    }

    if (err != 0)
    {
        //std::cout << "connect failed. socket:" << socket << " err:" << err << " networkType:" << GetNetworkTypeName(_networkType) << std::endl;
        _sock_close(socket);
        return false;
    }

    return true;
}

bool Network::CreateConnectObj(SOCKET socket, ObjectKey key, ConnectStateType iState)
{
    if (!CheckSocket(socket))
        return false;

    if (_connects[socket] != nullptr)
    {
        // 底层socket被重用，两个数据都要销毁，重来
        LOG_ERROR("Network::CreateConnectObj. socket is exist. socket:" << socket << " sn:" << _sn);
        RemoveConnectObj(socket);
        return false;
    }

    // Connect对象不进入System系统，Socket有重用的问题，所以它需要马上销毁，拖一帧也不行……
    auto pCollector = _pSystemManager->GetPoolCollector();
    auto pPool = dynamic_cast<DynamicObjectPool<ConnectObj>*>(pCollector->GetPool<ConnectObj>());
    ConnectObj* pConnectObj = pPool->MallocObject(_pSystemManager, socket, _networkType, key, iState);
    pConnectObj->SetParent(this);

    _connects[socket] = pConnectObj;
    _sockets.emplace(socket);

#ifdef LOG_TRACE_COMPONENT_OPEN
    const auto traceMsg = std::string("create. network type:").append(GetNetworkTypeName(_networkType));
    ComponentHelp::GetTraceComponent()->Trace(TraceType::Connector, socket, traceMsg);
#endif

#ifdef EPOLL
    AddEvent(_epfd, socket, EPOLLIN | EPOLLOUT | EPOLLET | EPOLLRDHUP);
#endif

    return true;
}

void Network::HandleDisconnect(Packet* pPacket)
{
    const auto socketKey = pPacket->GetSocketKey();
    if (socketKey.NetType != _networkType)
        return;

    const auto socket = socketKey.Socket;
    if (_connects[socket] == nullptr)
    {
        std::cout << "dis connect failed. socket not find." << pPacket << std::endl;
        return;
    }

    RemoveConnectObj(socket);
}

#ifdef EPOLL

void Network::AddEvent(int epollfd, int fd, int flag)
{
    struct epoll_event ev;
    ev.events = flag;
    ev.data.ptr = nullptr;
    ev.data.fd = fd;
    epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &ev);
}

void Network::ModifyEvent(int epollfd, int fd, int flag)
{
    struct epoll_event ev;
    ev.events = flag;
    ev.data.ptr = nullptr;
    ev.data.fd = fd;
    epoll_ctl(epollfd, EPOLL_CTL_MOD, fd, &ev);
}

void Network::DeleteEvent(int epollfd, int fd)
{
    epoll_ctl(epollfd, EPOLL_CTL_DEL, fd, nullptr);
}

void Network::InitEpoll()
{
    _epfd = epoll_create(MAX_CLIENT);
}

void Network::Epoll()
{
    const int nfds = epoll_wait(_epfd, _events, MAX_EVENT, 0);
    for (int index = 0; index < nfds; index++)
    {
        SOCKET socket = _events[index].data.fd;
        OnEpoll(socket, index);

        // 如果是master socket ，没有 connect obj        
        auto pObj = _connects[socket];
        if (pObj == nullptr)
            continue;

        if (_events[index].events & EPOLLRDHUP || _events[index].events & EPOLLERR || _events[index].events & EPOLLHUP)
        {
            RemoveConnectObj(socket);
            continue;
        }

        if (_events[index].events & EPOLLIN)
        {
            if (!pObj->Recv())
            {
                RemoveConnectObj(socket);
                continue;
            }
        }

        if (_events[index].events & EPOLLOUT)
        {
            if (!pObj->Send())
            {
                RemoveConnectObj(socket);
                continue;
            }

            ModifyEvent(_epfd, socket, EPOLLIN | EPOLLRDHUP);
        }
    }
}

#else

void Network::Select()
{
    for(auto socket:_sockets)
    {
        ConnectObj* pObj = _connects[socket];
        if (socket > _fdMax)
            _fdMax = socket;

        FD_SET(socket, &readfds);
        FD_SET(socket, &exceptfds);

        if (pObj->HasSendData())
            FD_SET(socket, &writefds);
    }

#if LOG_TRACE_COMPONENT_OPEN
    CheckPoint("select begin");
#endif

    struct timeval timeout;
    timeout.tv_sec = 0;
    timeout.tv_usec = 0;
    const int nfds = ::select(_fdMax + 1, &readfds, &writefds, &exceptfds, &timeout);
    if (nfds <= 0)
        return;

#if LOG_TRACE_COMPONENT_OPEN
    CheckPoint("select end");
#endif

    auto iter = _sockets.begin();
    while (iter != _sockets.end())
    {
        auto socket = *iter;
        auto pObj = _connects[socket];
        if (FD_ISSET(socket, &exceptfds))
        {
            std::cout << "socket except!! socket:" << socket << std::endl;
            RemoveConnectObjByItem(iter);
            continue;
        }

        if (FD_ISSET(socket, &readfds))
        {
            if (!pObj->Recv())
            {
                RemoveConnectObjByItem(iter);
                continue;
            }
        }

        if (FD_ISSET(socket, &writefds))
        {
            if (!pObj->Send())
            {
                RemoveConnectObjByItem(iter);
                continue;
            }
        }

        ++iter;
    }
}

#endif

void Network::OnNetworkUpdate()
{
    _sendMsgMutex.lock();
    if (_sendMsgList.CanSwap())
    {
        _sendMsgList.Swap();
    }
    _sendMsgMutex.unlock();

    auto pList = _sendMsgList.GetReaderCache();
    for (auto iter = pList->begin(); iter != pList->end(); ++iter)
    {
        Packet* pPacket = (*iter);
        auto socket = pPacket->GetSocketKey().Socket;
        const auto pObj = _connects[socket];
        if (pObj == nullptr)
        {
            LOG_ERROR("failed to send packet. can't find connect." << pPacket);
            DynamicPacketPool::GetInstance()->FreeObject(pPacket);
            continue;
        }

        // check
        if (pObj->GetObjectKey() != pPacket->GetObjectKey())
        {
            LOG_ERROR("failed to send packet. connect key is different. packet[" << pPacket << "] connect:[" << pObj << "]");
            DynamicPacketPool::GetInstance()->FreeObject(pPacket);
            continue;
        }

        pObj->SendPacket(pPacket);

#ifdef  EPOLL
        ModifyEvent(_epfd, socket, EPOLLIN | EPOLLOUT | EPOLLRDHUP);
#endif
    }
    pList->clear();
}

void Network::SendPacket(Packet*& pPacket)
{
    std::lock_guard<std::mutex> guard(_sendMsgMutex);

    if (pPacket->GetSocketKey().NetType != _networkType)
    {
        LOG_ERROR("failed to send packet. network type is different." << pPacket);
        return;
    }

    _sendMsgList.GetWriterCache()->emplace_back(pPacket);

#ifdef LOG_TRACE_COMPONENT_OPEN
    const google::protobuf::EnumDescriptor* descriptor = Proto::MsgId_descriptor();
    const auto name = descriptor->FindValueByNumber(pPacket->GetMsgId())->name();

    const auto traceMsg = std::string("send net.")
        .append(" sn:").append(std::to_string(pPacket->GetSN()))
        .append(" msgId:").append(name);
    ComponentHelp::GetTraceComponent()->Trace(TraceType::Packet, pPacket->GetSocketKey().Socket, traceMsg);
#endif
}


