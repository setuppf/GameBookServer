#pragma once
#include "libserver/entity.h"
#include "libplayer/world_base.h"

class WorldProxy :public IWorld, public Entity<WorldProxy>, public IAwakeFromPoolSystem<int, uint64>
{
public:
    void Awake(int worldId, uint64 lastWorldSn) override;
    void BackToPool() override;
};

