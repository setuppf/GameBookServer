#include "component.h"
#include "entity_system.h"
#include "entity.h"
#include "thread_mgr.h"
#include "object_pool.h"

void IComponent::SetParent(IEntity* pObj)
{
	_parent = pObj;
}

void IComponent::SetPool(IDynamicObjectPool* pPool)
{
	_pPool = pPool;
}

void IComponent::SetEntitySystem(EntitySystem* pSys)
{
	_pEntitySystem = pSys;
}

EntitySystem* IComponent::GetGlobalEntitySystem()
{
	return dynamic_cast<EntitySystem*>(ThreadMgr::GetInstance());
}

EntitySystem* IComponent::GetEntitySystem() const
{
	return _pEntitySystem;
}

void IComponent::ComponentBackToPool()
{
	_sn = 0;
	_parent = nullptr;
	_pEntitySystem = nullptr;
    _active = true;

	BackToPool();
	if (_pPool != nullptr)
		_pPool->FreeObject(this);
}
