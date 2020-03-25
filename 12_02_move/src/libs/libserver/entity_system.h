#pragma once
#include "component.h"

#include "disposable.h"
#include "component_factory.h"
#include "object_pool.h"
#include "component_collections.h"
#include "system_manager.h"
#include "log4_help.h"
#include "object_pool_collector.h"

class Packet;

class EntitySystem : public IDisposable
{
public:
    friend class ConsoleThreadComponent;
    EntitySystem(SystemManager* pMgr);
    virtual ~EntitySystem();

    template <class T, typename... TArgs>
    T* AddComponent(TArgs... args);

    template <class T, typename... TArgs>
    T* AddComponentWithParent(IEntity* pParent, uint64 sn, TArgs... args);

    template <typename... TArgs>
    IComponent* AddComponentByName(std::string className, uint64 sn, TArgs... args);

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
    pComponent->SetSystemManager(_systemManager);
}

template <class T, typename ... TArgs>
T* EntitySystem::AddComponent(TArgs... args)
{
    return AddComponentWithParent<T>(nullptr, 0, std::forward<TArgs>(args)...);
}

template <class T, typename ... TArgs>
T* EntitySystem::AddComponentWithParent(IEntity* pParent, uint64 sn, TArgs... args)
{
    auto pCollector = _systemManager->GetPoolCollector();
    auto pPool = dynamic_cast<DynamicObjectPool<T>*>(pCollector->GetPool<T>());
    T* pComponent = pPool->MallocObject(_systemManager, pParent, sn, std::forward<TArgs>(args)...);
    if (pComponent == nullptr)
        return nullptr;

    AddComponent(pComponent);
    return pComponent;
}

template<typename ...TArgs>
inline IComponent* EntitySystem::AddComponentByName(std::string className, uint64 sn, TArgs ...args)
{
    auto pObj = ComponentFactory<TArgs...>::GetInstance()->Create(_systemManager, className, sn, std::forward<TArgs>(args)...);
    if (pObj == nullptr)
        return nullptr;

    IComponent* pComponent = static_cast<IComponent*>(pObj);
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
