#include "network_locator.h"
#include "network_listen.h"
#include <algorithm>
#include <utility>
#include "message_system_help.h"
#include "component_help.h"
#include "socket_object.h"
#include "global.h"
#include "message_system.h"

void NetworkLocator::Awake()
{
    std::lock_guard<std::mutex> g1(_lock);

    _netIdentify.clear();
    _connectors.clear();
    _listens.clear();

    // message
    auto pMsgSystem = GetSystemManager()->GetMessageSystem();

    // http
    pMsgSystem->RegisterFunction(this, Proto::MsgId::MI_AppRegister, BindFunP1(this, &NetworkLocator::HandleAppRegister));
}

void NetworkLocator::BackToPool()
{
    std::lock_guard<std::mutex> g1(_lock);

    _netIdentify.clear();
    _connectors.clear();
    _listens.clear();
}

void NetworkLocator::AddConnectorLocator(INetwork* pNetwork, NetworkType networkType)
{
    std::lock_guard<std::mutex> guard(_lock);
    _connectors[networkType] = pNetwork;
}

void NetworkLocator::AddNetworkIdentify(SocketKey socket, ObjectKey objKey)
{
    _lock.lock();
    uint64 appKey = objKey.KeyValue.KeyInt64;
    const auto iter = _netIdentify.find(appKey);
    if (iter != _netIdentify.end())
    {
        LOG_WARN("connector loacator recv multiple socket.");
        _netIdentify.erase(appKey);
    }

    NetworkIdentify networkIdentify(socket, std::move(objKey));
    _netIdentify.insert(std::make_pair(appKey, networkIdentify));

    _lock.unlock();

    auto appType = GetTypeFromAppKey(appKey);
    auto appId = GetIdFromAppKey(appKey);
    LOG_DEBUG("connected appType:" << GetAppName(appType) << " appId:" << GetIdFromAppKey(appKey) << " " << (NetworkIdentify*)&networkIdentify);

    const auto pGlobal = Global::GetInstance();
    if ((pGlobal->GetCurAppType() & APP_APPMGR) == 0)
    {
        if (((appType & APP_APPMGR) != 0) || ((appType & APP_SPACE) != 0))
        {
            // 连接上了AppMgr，发送一个注册协议
            Proto::AppRegister proto;
            proto.set_type(pGlobal->GetCurAppType());
            proto.set_id(pGlobal->GetCurAppId());
            MessageSystemHelp::SendPacket(Proto::MsgId::MI_AppRegister, proto, appType, appId);
        }
    }
}

std::list<NetworkIdentify> NetworkLocator::GetAppNetworks(const APP_TYPE appType)
{
    std::lock_guard<std::mutex> guard(_lock);
    std::list<NetworkIdentify> rs;

    auto iter = _netIdentify.begin();
    while (iter != _netIdentify.end()) {
        iter = std::find_if(iter, _netIdentify.end(), [appType](auto pair)
            {
                auto appKey = pair.first;
                return GetTypeFromAppKey(appKey) == appType;
            });

        if (iter != _netIdentify.end())
        {
            rs.emplace_back(iter->second);
            ++iter;
        }
    }

    return rs;
}

void NetworkLocator::RemoveNetworkIdentify(uint64 appKey)
{
    _lock.lock();
    _netIdentify.erase(appKey);
    _lock.unlock();

    LOG_DEBUG("remove appType:" << GetAppName(GetTypeFromAppKey(appKey)) << " appId:" << GetIdFromAppKey(appKey));

    // 请求再次连接
    auto appType = GetTypeFromAppKey(appKey);
    auto appId = GetIdFromAppKey(appKey);
    const auto pYaml = ComponentHelp::GetYaml();
    const auto pCommonConfig = pYaml->GetIPEndPoint(appType, appId);
    ObjectKey key{ ObjectKeyType::App, {appKey, ""} };
    MessageSystemHelp::CreateConnect(NetworkType::TcpConnector, key, pCommonConfig->Ip, pCommonConfig->Port);
}

NetworkIdentify NetworkLocator::GetNetworkIdentify(const APP_TYPE appType, const int appId)
{
    std::lock_guard<std::mutex> guard(_lock);

    const auto appKey = GetAppKey(appType, appId);
    const auto iter = _netIdentify.find(appKey);
    if (iter == _netIdentify.end())
        return NetworkIdentify();

    return iter->second;
}

void NetworkLocator::AddListenLocator(INetwork* pNetwork, NetworkType networkType)
{
    std::lock_guard<std::mutex> guard(_lock);
    _listens[networkType] = pNetwork;
}

INetwork* NetworkLocator::GetNetwork(NetworkType networkType)
{
    std::lock_guard<std::mutex> guard(_lock);

    if (networkType == NetworkType::HttpListen || networkType == NetworkType::TcpListen)
    {
        auto iter = _listens.find(networkType);
        if (iter == _listens.end())
            return nullptr;

        return iter->second;
    }

    if (networkType == NetworkType::HttpConnector || networkType == NetworkType::TcpConnector)
    {
        auto iter = _connectors.find(networkType);
        if (iter == _connectors.end())
            return nullptr;

        return iter->second;
    }

    return nullptr;
}

void NetworkLocator::HandleAppRegister(Packet* pPacket)
{
    const auto proto = pPacket->ParseToProto<Proto::AppRegister>();
    const uint64 appKey = GetAppKey(proto.type(), proto.id());
    AddNetworkIdentify(pPacket->GetSocketKey(), ObjectKey{ ObjectKeyType::App, ObjectKeyValue { appKey, "" } });
}
