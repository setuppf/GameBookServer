#include "update_system.h"
#include "update_component.h"
#include "entity_system.h"

void UpdateSystem::Update(EntitySystem* pEntities)
{
    if (_pCollections == nullptr)
    {
        _pCollections = pEntities->GetComponentCollections<UpdateComponent>();
        if (_pCollections == nullptr)
            return;
    }

    _pCollections->Swap();
    const auto plists = _pCollections->GetAll();
    for (auto iter = plists->begin(); iter != plists->end(); ++iter)
    {
        const auto pUpdateComponent = dynamic_cast<UpdateComponent*>(iter->second);
        pUpdateComponent->Update();
    }
}
