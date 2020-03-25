#include "system_manager.h"

#include "create_component.h"

#include "message_system.h"
#include "entity_system.h"
#include "update_system.h"

#include "console_thread_component.h"
#include "object_pool_collector.h"
#include "timer_component.h"

#include <thread>

SystemManager::SystemManager()
{
    _pEntitySystem = new EntitySystem(this);
    _pMessageSystem = new MessageSystem(this);

    _pUpdateSystem = new UpdateSystem();
    _systems.emplace_back(_pUpdateSystem);

    //_systems.emplace_back(new DependenceSystem());
    //_systems.emplace_back(new StartSystem());

    // gen random seed 根据线程ID生成随机种子
    std::stringstream strStream;
    strStream << std::this_thread::get_id();
    std::string idstr = strStream.str();
    std::seed_seq seed1(idstr.begin(), idstr.end());
    std::minstd_rand0 generator(seed1);
    _pRandomEngine = new std::default_random_engine(generator());

    _pPoolCollector = new DynamicObjectPoolCollector(this);
}

void SystemManager::InitComponent(ThreadType threadType)
{
    _pEntitySystem->AddComponent<TimerComponent>();
    _pEntitySystem->AddComponent<CreateComponentC>();
    _pEntitySystem->AddComponent<ConsoleThreadComponent>(threadType);
}

void SystemManager::Update()
{
#if LOG_TRACE_COMPONENT_OPEN
    CheckBegin();
#endif

    _pPoolCollector->Update();
#if LOG_TRACE_COMPONENT_OPEN
    CheckPoint("pool");
#endif

    _pEntitySystem->Update();
#if LOG_TRACE_COMPONENT_OPEN
    CheckPoint("entity");
#endif

    _pMessageSystem->Update(_pEntitySystem);
#if LOG_TRACE_COMPONENT_OPEN
    CheckPoint("message");
#endif

    for (auto iter = _systems.begin(); iter != _systems.end(); ++iter)
    {
        auto pSys = (*iter);
        pSys->Update(_pEntitySystem);
#if LOG_TRACE_COMPONENT_OPEN
        CheckPoint(pSys->GetTypeName());
#endif
    }
}

void SystemManager::Dispose()
{
    for (auto one : _systems)
    {
        one->Dispose();
        delete one;
    }
    _systems.clear();

    delete _pRandomEngine;
    _pRandomEngine = nullptr;

    _pMessageSystem->Dispose();
    delete _pMessageSystem;
    _pMessageSystem = nullptr;

    _pEntitySystem->Dispose();
    delete _pEntitySystem;
    _pEntitySystem = nullptr;

    // 再销毁之前，再做一次Update，让use中的对象回到Free中
    _pPoolCollector->Update();
    _pPoolCollector->Dispose();
    delete _pPoolCollector;
    _pPoolCollector = nullptr;
}

std::default_random_engine* SystemManager::GetRandomEngine() const
{
    return _pRandomEngine;
}

void SystemManager::AddSystem(const std::string& name)
{
    auto pObj = ComponentFactory<>::GetInstance()->Create(nullptr, name, 0);
    if (pObj == nullptr)
    {
        LOG_ERROR("failed to create system.");
        return;
    }

    System* pSystem = static_cast<System*>(pObj);
    if (pSystem == nullptr)
    {
        LOG_ERROR("failed to create system.");
        return;
    }

    LOG_DEBUG("create system. name:" << name.c_str() << " thread id:" << std::this_thread::get_id());
    _systems.emplace_back(pSystem);
}

MessageSystem* SystemManager::GetMessageSystem() const
{
    return _pMessageSystem;
}

EntitySystem* SystemManager::GetEntitySystem() const
{
    return _pEntitySystem;
}

UpdateSystem* SystemManager::GetUpdateSystem() const
{
    return _pUpdateSystem;
}

DynamicObjectPoolCollector* SystemManager::GetPoolCollector() const
{
    return _pPoolCollector;
}
