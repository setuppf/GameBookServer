#pragma once
#include "libserver/entity.h"
#include "libserver/system.h"

#include "login_obj_mgr.h"

#include <json/reader.h>

class Account :public Entity<Account>, public IAwakeSystem<>
{
public:
    void Awake() override;
    virtual void BackToPool() override;

private:
    void SyncAppInfoToAppMgr();

    void HandleNetworkConnected(Packet* pPacket);
    void HandleNetworkDisconnect(Packet* pPacket);
    void SocketDisconnect(std::string account, NetworkIdentify* pIdentify);

    void HandleHttpOuterResponse(Packet* pPacket);

    void HandleAccountCheck(Packet* pPacket);
    void HandleQueryPlayerListRs(Packet* pPacket);

    void HandleCreatePlayer(Packet* pPacket);
    void HandleCreatePlayerRs(Packet* pPacket);

private:
    Proto::AccountCheckReturnCode ProcessMsg(Json::Value value) const;

private:
    LoginObjMgr _playerMgr;

    // http
    std::string _method{ "" };
    std::string _httpIp{ "" };
    int _httpPort{ 0 };
};

