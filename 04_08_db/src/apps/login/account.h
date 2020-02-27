#pragma once
#include "libserver/component.h"
#include "libserver/message_system.h"

#include "login_obj_mgr.h"

class Account :public Component<Account>, public IMessageSystem, public IAwakeFromPoolSystem<>
{
public:
    void AwakeFromPool() override {}
    void RegisterMsgFunction() override;
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

