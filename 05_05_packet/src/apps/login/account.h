#pragma once
#include "libserver/entity.h"
#include "libserver/system.h"

#include "login_obj_mgr.h"

class Account :public Entity<Account>, public IAwakeSystem<>
{
public:
    void Awake() override;
    virtual void BackToPool() override;

private:
    void HandleNetworkDisconnect(Packet* pPacket);

    void HandleAccountCheck(Packet* pPacket);
    void HandleAccountCheckToHttpRs(Packet* pPacket);
    void HandleQueryPlayerListRs(Packet* pPacket);

    void HandleCreatePlayer(Packet* pPacket);
    void HandleCreatePlayerRs(Packet* pPacket);

private:
    LoginObjMgr _playerMgr;
};

