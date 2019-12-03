#pragma once

#include <map>
#include <list>
#include <memory>
#include <algorithm>
#include <queue>

#include "component.h"
#include "object_pool.h"

class IEntity :public IComponent
{
public:
    virtual ~IEntity();
    void BackToPool() override;

    template <class T, typename... TArgs>
    void AddComponent(TArgs... args);

    template<class T>
    T* GetComponent();

private:
    void AddToSystem(IComponent* pObj);

protected:
    std::map<uint64, IComponent*> _components;
};

template <class T, typename... TArgs>
inline void IEntity::AddComponent(TArgs... args)
{
    auto pComponent = DynamicObjectPool<T>::GetInstance()->MallocObject(std::forward<TArgs>(args)...);
    AddToSystem(pComponent);
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
class Entity :public IEntity
{
public:
    virtual const char* GetTypeName();
    uint64 GetTypeHashCode();
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
