#pragma once
#include "libserver/entity.h"
#include "libserver/system.h"
#include "libserver/sync_component.h"
#include "libserver/socket_object.h"

#include <json/reader.h>
class Player;

class Account :public Entity<Account>, public IAwakeSystem<>, public SyncComponent
{
public:
    void Awake() override;
    virtual void BackToPool() override;

private:
    void SyncAppInfoToAppMgr();
    void HandleAppInfoListSync(Packet* pPacket);

    void HandleNetworkConnected(Packet* pPacket);
    void HandleNetworkDisconnect(Packet* pPacket);
    void SocketDisconnect(std::string account, NetworkIdentify* pIdentify);

    void HandleHttpOuterResponse(Packet* pPacket);

    void HandleAccountCheck(Packet* pPacket);
    void HandleAccountQueryOnlineToRedisRs(Packet* pPacket);
    void HandleQueryPlayerListRs(Packet* pPacket);

    void HandleCreatePlayer(Packet* pPacket);
    void HandleCreatePlayerRs(Packet* pPacket);

    void HandleSelectPlayer(Packet* pPacket);
    void RequestToken(Player* pPlayer) const;
    void HandleTokenToRedisRs(Packet* pPacket);

private:
    Proto::AccountCheckReturnCode ProcessMsg(Json::Value value) const;

private:
    // http
    std::string _method{ "" };
    std::string _httpIp{ "" };
    int _httpPort{ 0 };
};
