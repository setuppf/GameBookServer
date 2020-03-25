#pragma once
#include "component.h"
#include "global.h"
#include "cache_swap.h"
#include "disposable.h"
#include "component_factory.h"
#include "object_pool.h"
#include "component_collections.h"
#include "system_manager.h"
#include "log4_help.h"

class Packet;

class EntitySystem : public IDisposable
{
public:
    EntitySystem(SystemManager* pMgr);
    virtual ~EntitySystem();

    template <class T, typename... TArgs>
    T* AddComponent(TArgs... args);

    template <typename... TArgs>
    IComponent* AddComponentByName(std::string className, TArgs... args);

    template <class T>
    T* GetComponent();

    void RemoveComponent(IComponent* pObj);

    template<class T>
    ComponentCollections* GetComponentCollections();

    void Update();
    void Dispose() override;

private:
    template <class T>
    void AddComponent(T* pComponent);

    // 所有对象
    // <class mask sn, std::map<Component>*>
    std::map<uint64, ComponentCollections*> _objSystems;

private:
    SystemManager* _systemManager;
};

template<class T>
inline void EntitySystem::AddComponent(T* pComponent)
{
    const auto typeHashCode = pComponent->GetTypeHashCode();

#if LOG_SYSOBJ_OPEN
    LOG_SYSOBJ("*[sys] add obj. obj sn:" << pComponent->GetSN() << " type:" << pComponent->GetTypeName() << " thead id:" << std::this_thread::get_id());
#endif

    auto iter = _objSystems.find(typeHashCode);
    if (iter == _objSystems.end())
    {
        _objSystems[typeHashCode] = new ComponentCollections(pComponent->GetTypeName());
    }

    auto pEntities = _objSystems[typeHashCode];
    pEntities->Add(dynamic_cast<IComponent*>(pComponent));
}

template <class T, typename ... TArgs>
T* EntitySystem::AddComponent(TArgs... args)
{
    auto pComponent = DynamicObjectPool<T>::GetInstance()->MallocObject(_systemManager, std::forward<TArgs>(args)...);
    AddComponent(pComponent);
    return pComponent;
}

template<typename ...TArgs>
inline IComponent* EntitySystem::AddComponentByName(std::string className, TArgs ...args)
{
    auto pComponent = ComponentFactory<TArgs...>::GetInstance()->Create(_systemManager, className, std::forward<TArgs>(args)...);
    if (pComponent == nullptr)
        return nullptr;

    AddComponent(pComponent);
    return pComponent;
}

template <class T>
T* EntitySystem::GetComponent()
{
    const auto typeHashCode = typeid(T).hash_code();
    auto iter = _objSystems.find(typeHashCode);
    if (iter == _objSystems.end())
        return nullptr;

    return dynamic_cast<T*>(iter->second->Get());
}

template<class T>
inline ComponentCollections* EntitySystem::GetComponentCollections()
{
    const auto typeHashCode = typeid(T).hash_code();
    auto iter = _objSystems.find(typeHashCode);
    if (iter == _objSystems.end())
    {
        //LOG_WARN("GetComponentCollections failed. class name:" << className);
        return nullptr;
    }

    return iter->second;
}
