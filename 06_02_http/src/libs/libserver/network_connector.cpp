
#include <iostream>

#include "common.h"
#include "network_connector.h"
#include "network_locator.h"
#include "log4_help.h"
#include "yaml.h"
#include "thread_mgr.h"
#include "update_component.h"
#include "component_help.h"
#include "message_callback.h"
#include "message_component.h"

void NetworkConnector::Awake(int iType, int mixConnectAppType)
{
    _networkType = (NetworkType)iType;

    // 
    auto pNetworkLocator = ThreadMgr::GetInstance()->GetEntitySystem()->GetComponent<NetworkLocator>();
    pNetworkLocator->AddConnectorLocator(this, _networkType);

    // update
    auto pUpdateComponent = AddComponent<UpdateComponent>();
    pUpdateComponent->UpdataFunction = BindFunP0(this, &NetworkConnector::Update);

    // message
    auto pMsgCallBack = new MessageCallBackFunction();
    AddComponent<MessageComponent>(pMsgCallBack);
    pMsgCallBack->RegisterFunction(Proto::MsgId::MI_NetworkConnect, BindFunP1(this, &NetworkConnector::HandleNetworkConnect));
    pMsgCallBack->RegisterFunction(Proto::MsgId::MI_NetworkRequestDisconnect, BindFunP1(this, &NetworkConnector::HandleDisconnect));

#ifdef EPOLL
    std::cout << "epoll model. connector:" << GetNetworkTypeName(_networkType) << std::endl;
    InitEpoll();
#else
    std::cout << "select model. connector:" << GetNetworkTypeName(_networkType) << std::endl;
#endif

    if (_networkType == NetworkType::TcpConnector && mixConnectAppType > 0)
    {
        if ((mixConnectAppType & APP_APPMGR) != 0)
        {
            CreateConnector(APP_APPMGR, 0);
        }

        if ((mixConnectAppType & APP_DB_MGR) != 0)
        {
            CreateConnector(APP_DB_MGR, 0);
        }
    }
}

void NetworkConnector::CreateConnector(APP_TYPE appType, int appId)
{
    const auto pYaml = ComponentHelp::GetYaml();
    const auto pCommonConfig = pYaml->GetIPEndPoint(appType, appId);
    ConnectDetail* pDetail = new ConnectDetail({ ObjectKeyType::App, { GetAppKey(appType, appId), ""} }, pCommonConfig->Ip, pCommonConfig->Port);
    _connecting.AddObj(pDetail);
}


const char* NetworkConnector::GetTypeName()
{
    return typeid(NetworkConnector).name();
}

uint64 NetworkConnector::GetTypeHashCode()
{
    return typeid(NetworkConnector).hash_code();
}

bool NetworkConnector::Connect(ConnectDetail* pDetail)
{
    const int socket = CreateSocket();
    if (socket == INVALID_SOCKET)
        return false;

    sockaddr_in addr;
    memset(&addr, 0, sizeof(sockaddr_in));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(pDetail->Port);
    ::inet_pton(AF_INET, pDetail->Ip.c_str(), &addr.sin_addr.s_addr);

    const int rs = ::connect(socket, (struct sockaddr*) & addr, sizeof(sockaddr));
    if (rs == 0)
    {
        return CreateConnectObj(socket, pDetail->Key, ConnectStateType::Connected);
    }
    else
    {
        const auto socketError = _sock_err();
        if (!NetworkHelp::IsError(socketError))
        {
            // 未连接上，等待
#ifdef LOG_TRACE_COMPONENT_OPEN
            std::stringstream traceMsg;
            traceMsg << "create connect != 0 waiting, err=" << socketError;
            traceMsg << " network type:" << GetNetworkTypeName(_networkType);
            ComponentHelp::GetTraceComponent()->Trace(TraceType::Connector, socket, traceMsg.str());
#endif

            return CreateConnectObj(socket, pDetail->Key, ConnectStateType::Connecting);
        }
        else
        {
            // 未连接上，出错，重来
            LOG_WARN("failed to connect 2. ip:" << pDetail->Ip.c_str() << " port:" << pDetail->Port << " network sn:" << _sn << " socket:" << socket << " err:" << socketError);
            _sock_close(socket);
            return false;
        }
    }
}

void NetworkConnector::HandleNetworkConnect(Packet* pPacket)
{
    auto proto = pPacket->ParseToProto<Proto::NetworkConnect>();
    if (proto.network_type() != (int)_networkType)
        return;

    ObjectKey key;
    key.ParseFromProto(proto.key());
    ConnectDetail* pDetail = new ConnectDetail(key, proto.ip(), proto.port());
    _connecting.AddObj(pDetail);
}

#ifdef EPOLL

void NetworkConnector::Update()
{
    // 有新的连接请求
    if (_connecting.CanSwap())
        _connecting.Swap(nullptr);

    if (!_connecting.GetReaderCache()->empty())
    {
        auto pReader = _connecting.GetReaderCache();
        for (auto iter = pReader->begin(); iter != pReader->end(); ++iter)
        {
            if (Connect(iter->second))
            {
                _connecting.RemoveObj(iter->first);
            }
        }
    }

    Epoll();
    OnNetworkUpdate();
}

#else

void NetworkConnector::Update()
{
    // 有新的连接请求
    if (_connecting.CanSwap())
        _connecting.Swap(nullptr);

    if (!_connecting.GetReaderCache()->empty())
    {
        auto pReader = _connecting.GetReaderCache();
        for (auto iter = pReader->begin(); iter != pReader->end(); ++iter)
        {
            if (Connect(iter->second))
            {
                _connecting.RemoveObj(iter->first);
            }
        }
    }

    FD_ZERO(&readfds);
    FD_ZERO(&writefds);
    FD_ZERO(&exceptfds);

    _fdMax = 0;

    Select();
    OnNetworkUpdate();
}

#endif

