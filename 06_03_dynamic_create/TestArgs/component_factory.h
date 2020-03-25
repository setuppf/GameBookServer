#pragma once
#include <string>
#include <functional>
#include "component.h"
#include <map>

template<typename ...Targs>
class ComponentFactory {
public:
    static ComponentFactory<Targs...>* GetInstance() {
        if (_pInstance == nullptr) {
            _pInstance = new ComponentFactory<Targs...>();
        }
        return _pInstance;
    }

    bool Regist(const std::string& className, std::function<Component* (Targs... args)> pFunc) {
        if (_map.find(className) != _map.end())
            return false;

        _map.insert(std::make_pair(className, pFunc));
        return true;
    }

    Component* Create(const std::string className, Targs... args) {
        auto iter = _map.find(className);
        if (iter == _map.end()) {
            return nullptr;
        }
        else {
            return iter->second(std::forward<Targs>(args)...);
        }
    }

private:
    static ComponentFactory<Targs...>* _pInstance;
    std::map<std::string, std::function<Component* (Targs...)> > _map;
};

template<typename ...Targs>
ComponentFactory<Targs...>* ComponentFactory<Targs...>::_pInstance = nullptr;
