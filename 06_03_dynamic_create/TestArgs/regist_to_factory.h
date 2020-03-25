#pragma once
#include <typeinfo>
#include "component_factory.h"

template<typename T, typename...Targs>
class RegistToFactory {
public:
    RegistToFactory() {
        ComponentFactory<Targs...>::GetInstance()->Regist(typeid(T).name(), CreateComponent);
    }

    static T* CreateComponent(Targs... args) {
        return new T(std::forward<Targs>(args)...);
    }
};
