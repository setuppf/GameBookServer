#pragma once

#include "system.h"
#include "component_collections.h"

class UpdateSystem : virtual public ISystem<UpdateSystem>
{
public:
    void Update(EntitySystem* pEntities) override;

private:
    ComponentCollections* _pCollections{ nullptr };
};
