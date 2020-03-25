#pragma once
#include <typeinfo>

#include "component_factory.h"
#include "object_pool.h"

template<typename T, typename...Targs>
class RegistToFactory
{
public:
    RegistToFactory()
    {
        ComponentFactory<Targs...>::GetInstance()->Regist(typeid(T).name(), CreateComponent);
    }

    static T* CreateComponent(SystemManager* pSysMgr, uint64 sn, Targs... args)
    {
        auto pCollector = pSysMgr->GetPoolCollector();
        auto pPool = dynamic_cast<DynamicObjectPool<T>*>(pCollector->GetPool<T>());
        return pPool->MallocObject(pSysMgr, nullptr, sn, std::forward<Targs>(args)...);
    }
};

template<typename T, typename...Targs>
class RegistObject
{
public:
    RegistObject()
    {
        ComponentFactory<Targs...>::GetInstance()->Regist(typeid(T).name(), CreateComponent);
    }

    static T* CreateComponent(SystemManager* pSysMgr, uint64 sn, Targs... args)
    {
        return new T(std::forward<Targs>(args)...);
    }
};
