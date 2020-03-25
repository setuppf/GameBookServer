#pragma once

class ObjectBlock;

class IDynamicObjectPool
{
public:
    virtual ~IDynamicObjectPool() = default;
    virtual void Update() = 0;
    virtual void DestroyInstance() = 0;
    virtual void FreeObject(ObjectBlock* pObj) = 0;
};

