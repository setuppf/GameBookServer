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
public:
    IAwakeSystem() = default;
    virtual ~IAwakeSystem() = default;
    virtual void Awake(TArgs... args) = 0;
    static bool IsSingle() { return true; }
};

template <typename... TArgs>
class IAwakeFromPoolSystem : virtual public ISystem
{
public:
    IAwakeFromPoolSystem() = default;
    virtual ~IAwakeFromPoolSystem() = default;
    virtual void Awake(TArgs... args) = 0;
    static bool IsSingle() { return false; }
};
