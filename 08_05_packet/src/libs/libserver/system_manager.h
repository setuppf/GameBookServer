#pragma once

#include "disposable.h"
#include "system.h"
#include "common.h"
#include "thread_type.h"

#include <list>
#include <random>

class EntitySystem;
class MessageSystem;
class DynamicObjectPoolCollector;

class SystemManager : virtual public IDisposable
{
public:
    SystemManager();
    void InitComponent(ThreadType threadType);

    virtual void Update();
    void Dispose() override;

    MessageSystem* GetMessageSystem() const;
    EntitySystem* GetEntitySystem() const;
    DynamicObjectPoolCollector* GetPoolCollector() const;

    std::default_random_engine* GetRandomEngine() const;

protected:
    MessageSystem* _pMessageSystem;
    EntitySystem* _pEntitySystem;

    std::list<ISystem*> _systems;

    std::default_random_engine* _pRandomEngine;

    DynamicObjectPoolCollector* _pPoolCollector;
};
