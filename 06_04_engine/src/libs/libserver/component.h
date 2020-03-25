#pragma once
#include "sn_object.h"
#include "common.h"

class IEntity;
class EntitySystem;
class IDynamicObjectPool;

class IComponent : virtual public SnObject
{
public:
	friend class EntitySystem;

    virtual ~IComponent() = default;

    void SetPool(IDynamicObjectPool* pPool);
    void SetParent(IEntity* pObj);
    void SetEntitySystem(EntitySystem* pSys);

    bool IsActive() const { return _active; }

    template<class T>
    T* GetParent();

    static EntitySystem* GetGlobalEntitySystem();
    EntitySystem* GetEntitySystem() const;
    virtual void BackToPool() = 0;
    virtual void ComponentBackToPool();

protected:
    bool _active{ true };

private:
    IEntity* _parent{ nullptr };
    EntitySystem* _pEntitySystem{ nullptr };
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
	virtual const char* GetTypeName();
    uint64 GetTypeHashCode();
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
