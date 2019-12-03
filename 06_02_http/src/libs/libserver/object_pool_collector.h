#pragma once

#include <list>
#include <mutex>

#include "common.h"
#include "component.h"
#include "system.h"

#include "object_pool.h"

class SystemManager;

class DynamicObjectPoolCollector : public IDisposable
{
public:
    DynamicObjectPoolCollector(SystemManager* pSys);
    void Dispose() override;

    template<class T>
    IDynamicObjectPool* GetPool();

    void Update();
    void Show();

private:
    std::map<uint64, IDynamicObjectPool*> _pools;
    SystemManager* _pSystemManager{ nullptr };
};

template<class T>
IDynamicObjectPool* DynamicObjectPoolCollector::GetPool()
{
    const auto typeHashCode = typeid(T).hash_code();
    auto iter = _pools.find(typeHashCode);
    if (iter != _pools.end())
    {
        return iter->second;
    }

    auto pPool = new DynamicObjectPool<T>();
    _pools.insert(std::make_pair(typeHashCode, pPool));
    return pPool;
}
