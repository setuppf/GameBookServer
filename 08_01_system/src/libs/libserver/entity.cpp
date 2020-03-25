#include "entity.h"
#include "entity_system.h"

void IEntity::BackToPool() {
    for (auto pair : _components)
    {
        // 由 EntitySystem 去销毁
        const auto pEntitySystem = GetSystemManager()->GetEntitySystem();
        if (pEntitySystem != nullptr)
            pEntitySystem->RemoveComponent(pair.second);
        else
            pair.second->ComponentBackToPool();
    }

    _components.clear();
}

void IEntity::RemoveComponent(IComponent* pComponent)
{
    const auto typeHashCode = pComponent->GetTypeHashCode();
    _components.erase(typeHashCode);

    const auto pEntitySystem = GetSystemManager()->GetEntitySystem();
    if (pEntitySystem == nullptr)
    {
        pComponent->ComponentBackToPool();
    }
    else
    {
        // 由 EntitySystem 去销毁
        pEntitySystem->RemoveComponent(pComponent);
    }
}
