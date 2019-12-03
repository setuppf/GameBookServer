#include "component.h"
#include "entity.h"
#include "system_manager.h"
#include "thread_mgr.h"
#include "object_pool.h"

void IComponent::SetParent(IEntity* pObj)
{
    _parent = pObj;
}

void IComponent::SetSystemManager(SystemManager* pObj)
{
    _pSystemManager = pObj;
}

void IComponent::SetPool(IDynamicObjectPool* pPool)
{
    _pPool = pPool;
}

IEntity* IComponent::GetParent() const
{
    return _parent;
}

SystemManager* IComponent::GetSystemManager() const
{
    return _pSystemManager;
}

void IComponent::ComponentBackToPool()
{
    BackToPool();

    _sn = 0;
    _parent = nullptr;
    _pSystemManager = nullptr;
    _active = true;

    if (_pPool != nullptr)
    {
        _pPool->FreeObject(this);
        _pPool = nullptr;
    }
}
