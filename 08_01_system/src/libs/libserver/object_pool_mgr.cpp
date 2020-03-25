#include "object_pool_mgr.h"
#include "object_pool.h"

void DynamicObjectPoolMgr::AddPool(IDynamicObjectPool* pPool)
{
	std::lock_guard<std::mutex> guard(_lock);
	_pools.push_back(pPool);
}

void DynamicObjectPoolMgr::Update()
{
	std::lock_guard<std::mutex> guard(_lock);
	for (auto iter = _pools.begin(); iter != _pools.end(); ++iter)
	{
		(*iter)->Update();
	}
}

void DynamicObjectPoolMgr::Dispose()
{
	std::lock_guard<std::mutex> guard(_lock);
	for (auto pObj : _pools) 
	{
		pObj->DestroyInstance();
	}

	_pools.clear();
}
