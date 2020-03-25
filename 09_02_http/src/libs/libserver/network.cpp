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

    for (auto iter = _connects.begin(); iter != _connects.end(); ++iter)
    {
        auto pObj = iter->second;
        pObj->ComponentBackToPool();
    }
    _connects.clear();

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

    const auto iter = _connects.find(socket);
    if (iter != _connects.end())
    {
        // 底层socket被重用，两个数据都要销毁，重来
        LOG_ERROR("Network::CreateConnectObj. socket is exist. socket:" << socket << " sn:" << _sn);
        _connects[socket]->ComponentBackToPool();
        _connects.erase(iter);
        return false;
    }

    // Connect对象不进入System系统，Socket有重用的问题，所以它需要马上销毁，拖一帧也不行……
    auto pCollector = _pSystemManager->GetPoolCollector();
    auto pPool = dynamic_cast<DynamicObjectPool<ConnectObj>*>(pCollector->GetPool<ConnectObj>());
    ConnectObj* pConnectObj = pPool->MallocObject(_pSystemManager, socket, _networkType, key, iState);
    pConnectObj->SetParent(this);

    _connects[socket] = pConnectObj;

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

    auto iter = _connects.find(socketKey.Socket);
    if (iter == _connects.end())
    {
        std::cout << "dis connect failed. socket not find." << pPacket << std::endl;
        return;
    }

    RemoveConnectObj(iter);
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
        SOCKET fd = _events[index].data.fd;
        OnEpoll(fd, index);

        auto iter = _connects.find(fd);
        if (iter == _connects.end())
            continue;

        if (_events[index].events & EPOLLRDHUP || _events[index].events & EPOLLERR || _events[index].events & EPOLLHUP)
        {
            RemoveConnectObj(iter);
            continue;
        }

        if (_events[index].events & EPOLLIN)
        {
            if (!iter->second->Recv())
            {
                RemoveConnectObj(iter);
                continue;
            }
        }

        if (_events[index].events & EPOLLOUT)
        {
            if (!iter->second->Send())
            {
                RemoveConnectObj(iter);
                continue;
            }

            ModifyEvent(_epfd, iter->first, EPOLLIN | EPOLLRDHUP);
        }
    }
}

#else

void Network::Select()
{
    for (auto iter = _connects.begin(); iter != _connects.end(); ++iter)
    {
        if (iter->first > _fdMax)
            _fdMax = iter->first;

        FD_SET(iter->first, &readfds);
        FD_SET(iter->first, &exceptfds);
        FD_SET(iter->first, &writefds);
    }

    struct timeval timeout;
    timeout.tv_sec = 0;
    timeout.tv_usec = 0;
    const int nfds = ::select(_fdMax + 1, &readfds, &writefds, &exceptfds, &timeout);
    if (nfds <= 0)
        return;

    auto iter = _connects.begin();
    while (iter != _connects.end())
    {
        auto pObj = iter->second;
        if (FD_ISSET(iter->first, &exceptfds))
        {
            std::cout << "socket except!! socket:" << iter->first << std::endl;
            RemoveConnectObj(iter);
            continue;
        }

        if (FD_ISSET(iter->first, &readfds))
        {
            if (!pObj->Recv())
            {
                RemoveConnectObj(iter);
                continue;
            }
        }

        if (FD_ISSET(iter->first, &writefds))
        {
            if (!pObj->Send())
            {
                RemoveConnectObj(iter);
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
        auto socketKey = pPacket->GetSocketKey();
        auto iterObj = _connects.find(socketKey.Socket);
        if (iterObj == _connects.end())
        {
            LOG_ERROR("failed to send packet. can't find socket." << pPacket);
            DynamicPacketPool::GetInstance()->FreeObject(pPacket);
            continue;
        }

        // check
        const auto pObj = iterObj->second;
        if (pObj->GetObjectKey() != pPacket->GetObjectKey())
        {
            LOG_ERROR("failed to send packet. connect key is different. packet[" << pPacket << "] connect:[" << pObj << "]");
            DynamicPacketPool::GetInstance()->FreeObject(pPacket);
            continue;
        }

        pObj->SendPacket(pPacket);

#ifdef  EPOLL
        ModifyEvent(_epfd, socketKey.Socket, EPOLLIN | EPOLLOUT | EPOLLRDHUP);
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


