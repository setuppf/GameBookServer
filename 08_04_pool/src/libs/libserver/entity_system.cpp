#include "entity_system.h"
#include "log4.h"
#include "console.h"

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
    auto iterObj = _objSystems.find(typeHashCode);
    if (iterObj == _objSystems.end())
    {
        LOG_WARN("destroy class failed. class's name:" << pObj->GetTypeName() << " . not found class.");
        return;
    }

    iterObj->second->Remove(entitySn);

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
    std::set<uint64> baseClass;
    //baseClass.insert(typeid(TimeHeapComponent).hash_code());
    baseClass.insert(typeid(Log4).hash_code());
    baseClass.insert(typeid(Console).hash_code());

    for (auto one : _objSystems)
    {
        if (baseClass.find(one.first) != baseClass.end())
            continue;

        auto pCollections = one.second;
        pCollections->Dispose();
        delete pCollections;
    }

    // 对象池最后处理
    auto pPoolCollector = _systemManager->GetPoolCollector();

    // 再销毁之前，再做一次Update，让inuse中的对象回到Free中
    pPoolCollector->Update();
    pPoolCollector->Dispose();
    pPoolCollector->Show();

    _objSystems.clear();
}
