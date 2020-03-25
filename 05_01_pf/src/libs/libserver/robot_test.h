#pragma once
#include "thread_obj.h"

class RobotTest :public ThreadObject
{
public:
    bool Init() override;
    void RegisterMsgFunction() override;
    void Update() override;

private:
    void HandleTestBegin(Packet* pPacket);
    void HandleTestEnd(Packet* pPacket);

private:
    std::chrono::system_clock::time_point _start;
};

