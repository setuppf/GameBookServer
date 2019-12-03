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

    static T* CreateComponent(SystemManager* pSysMgr, Targs... args)
    {
        return DynamicObjectPool<T>::GetInstance()->MallocObject(pSysMgr, std::forward<Targs>(args)...);
    }
};
