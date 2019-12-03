#pragma once
#include <queue>
#include <iostream>
#include <sstream>
#include <list>

#include "sn_object.h"
#include "packet.h"
#include "object_pool_interface.h"
#include "cache_refresh.h"
#include "object_pool_mgr.h"
#include "system_manager.h"

template <typename T>
class DynamicObjectPool :public IDynamicObjectPool
{
public:
    static DynamicObjectPool<T>* GetInstance()
    {
        std::lock_guard<std::mutex> guard(_instanceLock);

        if (_pInstance == nullptr)
        {
            _pInstance = new DynamicObjectPool<T>();
            DynamicObjectPoolMgr::GetInstance()->AddPool(_pInstance);
        }

        return _pInstance;
    }

    void DestroyInstance() override
    {
        std::lock_guard<std::mutex> guard(_instanceLock);
        if (_pInstance == nullptr)
            return;

        delete _pInstance;
        _pInstance = nullptr;
    }

    DynamicObjectPool();
    ~DynamicObjectPool();

    template<typename ...Targs>
    T* MallocObject(SystemManager* pSysMgr, Targs... args);

    void Update() override;
    void FreeObject(IComponent* pObj) override;

    void Show();

private:
    void CreateOne();

private:
    std::queue<T*> _free;
    std::mutex _freeLock;

    std::mutex _inUseLock;
    CacheRefresh<T> _objInUse;

    static DynamicObjectPool<T>* _pInstance;
    static std::mutex _instanceLock;

#if _DEBUG
    int _totalCall{ 0 };
#endif
};

template <typename T>
DynamicObjectPool<T>* DynamicObjectPool<T>::_pInstance = nullptr;

template <typename T>
std::mutex DynamicObjectPool<T>::_instanceLock;


template <typename T>
DynamicObjectPool<T>::DynamicObjectPool()
{
}

template <typename T>
void DynamicObjectPool<T>::CreateOne()
{
    T* pObj = new T();
	pObj->SetPool(this);
    _free.push(pObj);
}

template <typename T>
DynamicObjectPool<T>::~DynamicObjectPool()
{
    // 再销毁之前，再做一次Update，让inuse中的对象回到Free中
    Update();

#if _DEBUG
    if (_objInUse.GetReaderCache()->size() > 0)
    {
        // 数据没有全部还回来，一定有问题~
        std::cout << "dispose object pool error. type:" << typeid(T).name() << " inUse size:" << _objInUse.GetReaderCache()->size() << " free size:" << _free.size() << std::endl;
    }
#endif

    std::cout << "dispose object pool. type:" << typeid(T).name() << std::endl;

    while (_free.size() > 0)
    {
        auto iter = _free.front();
        delete iter;
        _free.pop();
    }
}

template <typename T>
template <typename ... Targs>
T* DynamicObjectPool<T>::MallocObject(SystemManager* pSysMgr, Targs... args)
{
    _freeLock.lock();
    if (_free.size() == 0)
    {
        CreateOne();
        //std::cout << "+ obj. object pool. type:" << typeid(T).name() << std::endl;
    }

#if _DEBUG
    _totalCall++;
#endif

    auto pObj = _free.front();
    _free.pop();
    _freeLock.unlock();

    pObj->ResetSN();    
    pObj->SetSystemManager(pSysMgr);

    pObj->AwakeFromPool(std::forward<Targs>(args)...);

    _inUseLock.lock();
    _objInUse.GetAddCache()->push_back(pObj);
    _inUseLock.unlock();

    return pObj;
}

template <typename T>
void DynamicObjectPool<T>::Update()
{
    std::list<T*> freeObjs;
    _inUseLock.lock();
    if (_objInUse.CanSwap())
    {
        freeObjs = _objInUse.Swap();
    }
    _inUseLock.unlock();

    std::lock_guard<std::mutex> guard(_freeLock);
    for (auto one : freeObjs)
    {
        _free.push(one);
    }
}

template<typename T>
inline void DynamicObjectPool<T>::FreeObject(IComponent* pObj)
{
    std::lock_guard<std::mutex> guard(_inUseLock);
    _objInUse.GetRemoveCache()->emplace_back(dynamic_cast<T*>(pObj));
}

template <typename T>
void DynamicObjectPool<T>::Show()
{
    std::lock_guard<std::mutex> guard(_freeLock);
    std::lock_guard<std::mutex> guardInUse(_inUseLock);
    auto count = _objInUse.GetReaderCache()->size() + _objInUse.GetAddCache()->size() + _objInUse.GetRemoveCache()->size();

    std::stringstream log;
    log << "*************************** " << "\n";
    log << "pool total count:\t" << _free.size() + count << "\n";
    log << "free count:\t\t" << _free.size() << "\n";
    log << "in use count:\t" << count << "\n";

#if _DEBUG
    log << "total call:\t" << _totalCall << "\n";
#endif

    std::cout << log.str() << std::endl;
}

