#pragma once
#include "disposable.h"

class EntitySystem;
class ISystem :public IDisposable
{
protected:
    ISystem() = default;

public:
    virtual ~ISystem() = default;
    void Dispose() override {}
    virtual void Update(EntitySystem* pEntities) {}
};

template <typename... TArgs>
class IAwakeSystem : virtual public ISystem
{
protected:
    IAwakeSystem() = default;

public:
    virtual ~IAwakeSystem() = default;
    virtual void Awake(TArgs... args) = 0;
};

template <typename... TArgs>
class IAwakeFromPoolSystem : virtual public ISystem
{
protected:
    IAwakeFromPoolSystem() = default;

public:
    virtual ~IAwakeFromPoolSystem() = default;
    virtual void AwakeFromPool(TArgs... args) = 0;
};
