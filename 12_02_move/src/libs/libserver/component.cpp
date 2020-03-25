#include "component.h"
#include "entity.h"
#include "system_manager.h"
#include "thread_mgr.h"
#include "object_pool.h"
#include "timer_component.h"
#include "message_system.h"

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

    if (!_timers.empty()) 
    {
        auto pTimer = _pSystemManager->GetEntitySystem()->GetComponent<TimerComponent>();
        if (pTimer != nullptr)
            pTimer->Remove(_timers);

        _timers.clear();
    }

    if (_pPool != nullptr)
    {
        _pPool->FreeObject(this);
        _pPool = nullptr;
    }

    // ´Ó messageSystemÖÐÒÆ³ý
    if (_pSystemManager != nullptr && _pSystemManager->GetMessageSystem() != nullptr)
    {
        _pSystemManager->GetMessageSystem()->RemoveFunction(this);
    }

    _sn = 0;
    _parent = nullptr;
    _pSystemManager = nullptr;
}

void IComponent::AddTimer(const int total, const int durations, const bool immediateDo, const int immediateDoDelaySecond, TimerHandleFunction handler)
{
    auto obj = GetSystemManager()->GetEntitySystem()->GetComponent<TimerComponent>();
    const auto timer = obj->Add(total, durations, immediateDo, immediateDoDelaySecond, std::move(handler));
    _timers.push_back(timer);
}
