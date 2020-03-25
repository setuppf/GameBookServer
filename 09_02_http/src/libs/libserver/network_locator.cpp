#include "network_locator.h"
#include "network_listen.h"
#include <algorithm>
#include <utility>
#include "message_component.h"
#include "message_system_help.h"
#include "component_help.h"

void NetworkLocator::Awake()
{
    std::lock_guard<std::mutex> guard(_lock);
    _netIdentify.clear();
    _connectors.clear();
    _listens.clear();
}

void NetworkLocator::BackToPool()
{
    std::lock_guard<std::mutex> guard(_lock);
    _netIdentify.clear();
	_connectors.clear();
	_listens.clear();
}

void NetworkLocator::AddConnectorLocator(INetwork* pNetwork, NetworkType networkType)
{
    std::lock_guard<std::mutex> guard(_lock);
    _connectors[networkType] = pNetwork;
}

void NetworkLocator::AddNetworkIdentify(uint64 appKey, SocketKey socket, ObjectKey objKey)
{
	std::lock_guard<std::mutex> guard(_lock);
	const auto iter = _netIdentify.find(appKey);
	if (iter != _netIdentify.end())
	{
        LOG_WARN("connector loacator recv multiple socket.");
        _netIdentify.erase(appKey);
	}

    NetworkIdentify networkIdentify(socket, std::move(objKey));
    _netIdentify.insert(std::make_pair(appKey, networkIdentify));

    LOG_DEBUG("connected appType:" << GetTypeFromAppKey(appKey) << " appId:" << GetIdFromAppKey(appKey) << &networkIdentify);
}

void NetworkLocator::RemoveNetworkIdentify(uint64 appKey)
{
    std::lock_guard<std::mutex> guard(_lock);

    auto appType = GetTypeFromAppKey(appKey);
    auto appId = GetIdFromAppKey(appKey);
    LOG_DEBUG("dis connect. appType:" << GetAppName(appType) << " appId:" << appId << &(_netIdentify[appKey]));
    _netIdentify.erase(appKey);

    // 请求再次连接
    Proto::NetworkConnect protoConn;
    protoConn.set_network_type((int)NetworkType::TcpConnector);

    ObjectKey key{ ObjectKeyType::App, {appKey, ""} };
    key.SerializeToProto(protoConn.mutable_key());

    const auto pYaml = ComponentHelp::GetYaml();
    const auto pCommonConfig = pYaml->GetIPEndPoint(appType, appId);
    protoConn.set_ip(pCommonConfig->Ip.c_str());
    protoConn.set_port(pCommonConfig->Port);
    MessageSystemHelp::DispatchPacket(Proto::MsgId::MI_NetworkConnect, protoConn, nullptr);
}

INetwork* NetworkLocator::GetConnector(NetworkType networkType)
{
    std::lock_guard<std::mutex> guard(_lock);
    auto iter = _connectors.find(networkType);
    if (iter == _connectors.end())
        return nullptr;

    return iter->second;
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

INetwork* NetworkLocator::GetListen(NetworkType networkType)
{
	std::lock_guard<std::mutex> guard(_lock);
	auto iter = _listens.find(networkType);
	if (iter == _listens.end())
		return nullptr;

	return iter->second;
}
