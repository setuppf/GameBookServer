#pragma once

#include "ComponentFactory.h"

template<typename T, typename...Targs>
class RegistCreator {
public:
    RegistCreator() {
        std::string strTypeName = typeid(T).name();
        ComponentFactory<Targs...>::Instance()->Regist(strTypeName, CreateObject);
    }

    static T* CreateObject(Targs&& ... args) {
        return new T(std::forward<Targs>(args)...);
    }

};
