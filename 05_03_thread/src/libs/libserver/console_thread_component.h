#pragma once
#include "system.h"
#include "entity.h"
#include "thread_type.h"

class Packet;

class ConsoleThreadComponent :public Entity<ConsoleThreadComponent>, public IAwakeFromPoolSystem<ThreadType>
{
public:
    void AwakeFromPool(ThreadType iType);
    void BackToPool();

private:
    void HandleCmdShowThreadEntites(Packet* pPacket);

private:
    ThreadType _threadType;
};
