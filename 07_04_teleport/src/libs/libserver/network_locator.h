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
    void AddNetworkIdentify(SocketKey* pSocket, uint64 appKey);
    std::list<NetIdentify> GetAppNetworks(const APP_TYPE appType);
    void RemoveNetworkIdentify(uint64 appKey);

    NetIdentify GetNetworkIdentify(const APP_TYPE appType, const int appId);

    // listen
    void AddListenLocator(INetwork* pNetwork, NetworkType networkType);

    //
    INetwork* GetNetwork(NetworkType networkType);

protected:
    void HandleAppRegister(Packet* pPacket);
    void HandleNetworkDisconnect(Packet* pPacket);

private:
    std::mutex _lock;

    // <apptype + appId, NetIdentify>
    std::map<uint64, NetIdentify> _netIdentify;

    // connector
    std::map<NetworkType, INetwork*> _connectors;

    // 可能存在两个listen, tcp 和 http 同时存在
    std::map<NetworkType, INetwork*> _listens;
};

