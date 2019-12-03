#pragma once
#include "sn_object.h"
#include "common.h"

#include <functional>
#include <list>

class IEntity;
class IDynamicObjectPool;
class SystemManager;

using TimerHandleFunction = std::function<void(void)>;

class IComponent : virtual public SnObject
{
public:
    friend class EntitySystem;

    virtual ~IComponent() = default;

    void SetPool(IDynamicObjectPool* pPool);
    void SetParent(IEntity* pObj);
    void SetSystemManager(SystemManager* pObj);

    template<class T>
    T* GetParent();

    IEntity* GetParent() const;

    SystemManager* GetSystemManager() const;
    virtual void BackToPool() = 0;
    virtual void ComponentBackToPool();

    virtual const char* GetTypeName() = 0;
    virtual uint64 GetTypeHashCode() = 0;

protected:
    void AddTimer(const int total, const int durations, const bool immediateDo, const int immediateDoDelaySecond, TimerHandleFunction handler);
    std::list<uint64> _timers;

    IEntity* _parent{ nullptr };
    SystemManager* _pSystemManager{ nullptr };
    IDynamicObjectPool* _pPool{ nullptr };
};

template <class T>
T* IComponent::GetParent()
{
    return dynamic_cast<T*>(_parent);
}

template<class T>
class Component :public IComponent
{
public:
    const char* GetTypeName() override;
    uint64 GetTypeHashCode() override;
};

template <class T>
const char* Component<T>::GetTypeName()
{
    return typeid(T).name();
}

template <class T>
uint64 Component<T>::GetTypeHashCode()
{
    return typeid(T).hash_code();
}
