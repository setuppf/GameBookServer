#pragma once
#include "libserver/system.h"
#include "libserver/entity.h"

class WorldProxyComponentGather :public Entity<WorldProxyComponentGather>, public IAwakeFromPoolSystem<>
{
public:
    void Awake() override;
    void BackToPool() override;

private:
    void SyncWorldInfoToGather();

private:
    uint64 _worldSn{ 0 };
    int _worldId{ 0 };
};

