#pragma once

#include "component.h"

class IDynamicObjectPool
{
public:
    virtual ~IDynamicObjectPool() = default;

    virtual void Update() = 0;
    virtual void FreeObject(IComponent* pObj) = 0;
    virtual void Show() = 0;
};
