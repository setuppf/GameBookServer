#include "entity_system.h"
#include "log4.h"
#include "console.h"
#include "message_system.h"

EntitySystem::EntitySystem(SystemManager* pMgr)
{
    _systemManager = pMgr;
}

EntitySystem::~EntitySystem()
{
}

void EntitySystem::RemoveComponent(IComponent* pObj)
{
    const auto entitySn = pObj->GetSN();

    const auto typeHashCode = pObj->GetTypeHashCode();
    const auto iterObj = _objSystems.find(typeHashCode);
    if (iterObj == _objSystems.end())
    {
        LOG_WARN("destroy class failed. class's name:" << pObj->GetTypeName() << " . not found class.");
        return;
    }

    ComponentCollections* pCollector = iterObj->second;
    pCollector->Remove(entitySn);

#if LOG_SYSOBJ_OPEN
    LOG_SYSOBJ("*[sys] remove obj. obj sn:" << pObj->GetSN() << " type:" << pObj->GetTypeName() << " thead id:" << std::this_thread::get_id());
#endif
}

void EntitySystem::Update()
{
    for (auto iter : _objSystems)
    {
        iter.second->Swap();
    }
}

void EntitySystem::Dispose()
{
    // 拆成两次销毁
    // 1. Dispose 释放全部组件时,ComponentCollections有交叉引用
    // 2. delete 
    for (const auto one : _objSystems)
    {
        auto pCollections = one.second;
        pCollections->Dispose();
    }
    
    for (auto one : _objSystems) 
    {       
        delete one.second;
    }
    _objSystems.clear();
}
