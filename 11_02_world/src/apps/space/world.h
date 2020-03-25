#pragma once
#include "libserver/system.h"
#include "libplayer/world_base.h"
#include "libserver/entity.h"

class World:public Entity<World>, public IWorld, public IAwakeFromPoolSystem<int>
{
public:
    void Awake(int worldId) override;
    void BackToPool() override;

private:
    void SyncWorldToGather();
};

