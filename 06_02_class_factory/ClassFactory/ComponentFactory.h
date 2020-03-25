#pragma once
#include <string>
#include <functional>
#include "Component.h"
#include <unordered_map>

template<typename ...Targs>
class ComponentFactory {
public:
    static ComponentFactory* Instance() {
        if (nullptr == _instance) {
            _instance = new ComponentFactory();
        }
        return(_instance);
    }

    bool Regist(const std::string& strTypeName, std::function<Component* (Targs&& ... args)> pFunc);
    Component* Create(const std::string& strTypeName, Targs&& ... args);

private:
    ComponentFactory() { };

    static ComponentFactory<Targs...>* _instance;
    std::unordered_map<std::string, std::function<Component* (Targs&& ...)> > _map;
};


template<typename ...Targs>
ComponentFactory<Targs...>* ComponentFactory<Targs...>::_instance = nullptr;

template<typename ...Targs>
bool ComponentFactory<Targs...>::Regist(const std::string& strTypeName, std::function<Component* (Targs&& ... args)> pFunc) {
    if (nullptr == pFunc) {
        return (false);
    }

    const bool bReg = _map.insert(std::make_pair(strTypeName, pFunc)).second;
    return (bReg);
}

template<typename ...Targs>
Component* ComponentFactory<Targs...>::Create(const std::string& strTypeName, Targs&& ... args) {
    auto iter = _map.find(strTypeName);
    if (iter == _map.end()) {
        return (nullptr);
    }
    else {
        return (iter->second(std::forward<Targs>(args)...));
    }
}