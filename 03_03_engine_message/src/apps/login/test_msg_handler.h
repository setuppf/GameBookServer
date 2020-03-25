#pragma once
#include "libserver/thread_obj.h"

class TestMsgHandler :public ThreadObject
{
public:
    bool Init() override;
    void RegisterMsgFunction() override;
    void Update() override;

private:
    void HandleMsg(Packet* pPacket);

};

