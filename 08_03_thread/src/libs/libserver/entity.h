#pragma once

#include <map>
#include <list>
#include <memory>
#include <algorithm>
#include <queue>

#include "component.h"
#include "object_pool.h"
#include "entity_system.h"

class IEntity :public IComponent
{
public:
    virtual ~IEntity() = default;
    void BackToPool() override;

    template <class T, typename... TArgs>
    T* AddComponent(TArgs... args);
    void AddComponent(IComponent* pComponent);

    template<class T>
    T* GetComponent();

    template<class T>
    void RemoveComponent();

    void RemoveComponent(IComponent* pObj);

protected:
    std::map<uint64, IComponent*> _components;
};

template <class T, typename... TArgs>
inline T* IEntity::AddComponent(TArgs... args)
{
    auto pSystemManager = GetSystemManager();
    T* pComponent;
    if (pSystemManager != nullptr) 
    {
        pComponent = GetSystemManager()->GetEntitySystem()->AddComponent<T>(std::forward<TArgs>(args)...);
    }
    else {
        pComponent = DynamicObjectPool<T>::GetInstance()->MallocObject(nullptr, std::forward<TArgs>(args)...);
    }

    AddComponent(pComponent);
    return pComponent;
}

template<class T>
T* IEntity::GetComponent()
{
    auto iter = std::find_if(_components.begin(), _components.end(), [](auto pair)
        {
            if (dynamic_cast<T*>(pair.second) != nullptr)
                return true;

            return false;
        });

    if (iter == _components.end())
        return nullptr;

    return dynamic_cast<T*>(iter->second);
}

template<class T>
void IEntity::RemoveComponent()
{
    // 先删除本地组件数据
    const auto typeHashCode = typeid(T).hash_code();
    const auto iter = _components.find(typeHashCode);
    if (iter == _components.end())
    {
        LOG_ERROR("Entity RemoveComponent error. not find. className:" << typeid(T).name());
        return;
    }

    auto pComponent = iter->second;
    RemoveComponent(pComponent);
}

template<class T>
class Entity :public IEntity
{
public:
    const char* GetTypeName() override;
    uint64 GetTypeHashCode() override;
};

template <class T>
const char* Entity<T>::GetTypeName()
{
    return typeid(T).name();
}

template <class T>
uint64 Entity<T>::GetTypeHashCode()
{
    return typeid(T).hash_code();
}
