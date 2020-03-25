#pragma once

#include "disposable.h"
#include "component.h"

class IDynamicObjectPool:public IDisposable
{
public:
    virtual void Update() = 0;
    virtual void FreeObject(IComponent* pObj) = 0;
    virtual void Show() = 0;
};
