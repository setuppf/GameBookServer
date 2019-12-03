#pragma once
#include "system.h"
#include "entity.h"
#include "thread_type.h"

class Packet;

class ConsoleThreadComponent :public Entity<ConsoleThreadComponent>, public IAwakeSystem<ThreadType>
{
public:
    void Awake(ThreadType iType);
    void BackToPool();

private:
    void HandleCmdThread(Packet* pPacket);
    void HandleCmdThreadEntites(Packet* pPacket);
    void HandleCmdThreadPool(Packet* pPacket);
    void HandleCmdThreadConnect(Packet* pPacket);

private:
    ThreadType _threadType;

    static std::mutex _show_lock;
};
