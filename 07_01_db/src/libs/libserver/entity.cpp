#include "entity.h"
#include "entity_system.h"

IEntity::~IEntity()
{
    BackToPool();
}

void IEntity::BackToPool() {
    for (const auto& one : _components) {
        one.second->ComponentBackToPool();
    }

    _components.clear();
}

void IEntity::AddToSystem(IComponent* pComponent)
{
    pComponent->SetParent(this);

    _components.insert(std::make_pair(pComponent->GetSN(), pComponent));
    GetEntitySystem()->AddToSystem(pComponent);
}
