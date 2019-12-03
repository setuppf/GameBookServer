#pragma once
#include "libserver/system.h"
#include "libserver/entity.h"

class ConsoleAppComponent :public Entity<ConsoleAppComponent>, public IAwakeSystem<>
{
public:
    void Awake();
    void BackToPool();

private:
    void HandleCmdApp(Packet* pPacket);
    void HandleCmdAppInfo();
};

