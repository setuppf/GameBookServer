#pragma once
#include "network.h"
#include "system.h"
#include "app_type.h"
#include "socket_object.h"

class NetworkLocator : public Entity<NetworkLocator>, public IAwakeSystem<>
{
public:
    void Awake() override;;
    void BackToPool() override;

    // connect
    void AddConnectorLocator(INetwork* pNetwork, NetworkType networkType);
    void AddNetworkIdentify(uint64 appKey, SocketKey socket, ObjectKey objKey);
    void RemoveNetworkIdentify(uint64 appKey);

    INetwork* GetConnector(NetworkType networkType);
    NetworkIdentify GetNetworkIdentify(const APP_TYPE appType, const int appId);

    // listen
    void AddListenLocator(INetwork* pNetwork, NetworkType networkType);
    INetwork* GetListen(NetworkType networkType);

private:
    std::mutex _lock;

    // <apptype + appId, NetworkIdentify>
    std::map<uint64, NetworkIdentify> _netIdentify;

    // connector
    std::map<NetworkType, INetwork*> _connectors;

    // 可能存在两个listen, tcp 和 http 同时存在
    std::map<NetworkType, INetwork*> _listens;
};
