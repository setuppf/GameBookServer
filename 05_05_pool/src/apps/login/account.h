#pragma once
#include "libserver/thread_obj.h"
#include "login_obj_mgr.h"

class Account :public ThreadObject
{
public:
	bool Init() override;
	void RegisterMsgFunction() override;
	void Update() override;

private:
    void HandleNetworkDisconnect(Packet* pPacket);
    void HandleAccountCheck(Packet* pPacket);
    void HandleAccountCheckToHttpRs(Packet* pPacket);

private:
	LoginObjMgr _playerMgr;
};

 