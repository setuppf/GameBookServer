#pragma once
#include "disposable.h"
#include "common.h"

class EntitySystem;

class System :public IDisposable
{
public:
    virtual ~System() = default;
    void Dispose() override {}

    virtual const char* GetTypeName() { return ""; }
    virtual uint64 GetTypeHashCode() { return 0; }
    virtual void Update(EntitySystem* pEntities) {}
};

template<class T>
class ISystem :public System
{
public:
    virtual ~ISystem() = default;
    virtual const char* GetTypeName() override { return typeid(T).name(); }
    virtual uint64 GetTypeHashCode() override { return typeid(T).hash_code(); }
};

class IAwakeSystemBase :public IDisposable
{
public:
    void Dispose() override {}
    static bool IsSingle() { return true; }
};

template <typename... TArgs>
class IAwakeSystem :virtual public IAwakeSystemBase
{
public:
    IAwakeSystem() = default;
    virtual ~IAwakeSystem() = default;
    virtual void Awake(TArgs... args) = 0;
};

class IAwakeFromPoolSystemBase :public IDisposable
{
public:
    void Dispose() override {}
    static bool IsSingle() { return false; }
};

template <typename... TArgs>
class IAwakeFromPoolSystem :virtual public IAwakeFromPoolSystemBase
{
public:
    IAwakeFromPoolSystem() = default;
    virtual ~IAwakeFromPoolSystem() = default;
    virtual void Awake(TArgs... args) = 0;
};
