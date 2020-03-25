#pragma once
#include "libserver/system.h"
#include "libserver/util_time.h"
#include "libserver/component_collections.h"

class MoveSystem : public ISystem<MoveSystem>
{
public:
    MoveSystem();
    void Update(EntitySystem* pEntities) override;

private:
    timeutil::Time _lastTime;
    ComponentCollections* _pCollections{ nullptr };
};

