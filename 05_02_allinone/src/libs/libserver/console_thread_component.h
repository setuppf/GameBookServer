#pragma once
#include "system.h"
#include "entity.h"

class Packet;

class ConsoleThreadComponent :public Entity<ConsoleThreadComponent>, public IAwakeFromPoolSystem<>
{
public:
    void AwakeFromPool();
    void BackToPool();

private:
    void HandleCmdShowThreadEntites(Packet* pPacket);
};
