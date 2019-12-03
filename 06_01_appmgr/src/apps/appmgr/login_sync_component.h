#pragma once

#include "libserver/sync_component.h"
#include "libserver/system.h"

class LoginSyncComponent :public SyncComponent, public IAwakeSystem<>
{
public:
    void Awake() override;
    void BackToPool() override;
};

