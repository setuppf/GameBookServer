#pragma once

#include <map>
#include <list>
#include <memory>
#include <algorithm>
#include <queue>

#include "component.h"

class Entity :public Component
{
public:
    void Dispose() override;

    void AddComponent(Component* obj);

    template<class T>
    T* GetComponent();

protected:
    std::map<long, Component*> _components;
};

template<class T>
T* Entity::GetComponent()
{
    auto iter = std::find_if(_components.begin(), _components.end(), [](std::pair<long, Component*> one)
        {
            if (dynamic_cast<T>(one.second) != nullptr)
                return true;

            return false;
        });

    if (iter == _components.end())
        return nullptr;

    return dynamic_cast<T>(iter->second);
}

