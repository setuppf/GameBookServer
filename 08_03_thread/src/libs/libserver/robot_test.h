#pragma once
#include "entity.h"
#include "system.h"

class Packet;

class RobotTest :public Entity<RobotTest>, public IAwakeFromPoolSystem<>
{
public:
    void AwakeFromPool() override;
    void BackToPool() override {};

private:
    void HandleTestBegin(Packet* pPacket);
    void HandleTestEnd(Packet* pPacket);

private:
    std::chrono::system_clock::time_point _start;
};

