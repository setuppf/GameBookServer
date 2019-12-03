#include "entity_system.h"

bool EntitySystem::Update()
{
    // ³õÊ¼»¯
    while (_initializeSystems.size() > 0)
    {
        auto pComponent = _initializeSystems.front();
        pComponent->Initialize();
        _initializeSystems.pop_front();

        const auto objUpdate = dynamic_cast<IUpdateSystem*>(pComponent);
        if (objUpdate != nullptr)
            _updateSystems.push_back(objUpdate);

    }

    for (auto& iter : _updateSystems)
    {
        iter->Update();
    }

    return true;
}

void EntitySystem::Dispose()
{
    while (_initializeSystems.size() > 0)
    {
        auto pComponent = dynamic_cast<Component*>(_initializeSystems.front());
        pComponent->Dispose();
        _initializeSystems.pop_front();
    }

    for (auto& iter : _updateSystems)
    {
        auto pComponent = dynamic_cast<Component*>(iter);
        pComponent->Dispose();
    }

    _updateSystems.clear();
}
