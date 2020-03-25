#pragma once
#include "libserver/thread_obj.h"
#include "player_mgr.h"

class Account :public ThreadObject
{
public:
    bool Init() override;
    void RegisterMsgFunction() override;
    void Update() override;

private:
    void HandleAccountCheck(Packet* pPacket);
    void HandleAccountCheckToHttpRs(Packet* pPacket);

private:
    PlayerMgr _playerMgr;
};

