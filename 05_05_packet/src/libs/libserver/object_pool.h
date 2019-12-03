#pragma once
#include <queue>
#include <iostream>
#include <sstream>
#include <iomanip>

#include "sn_object.h"
#include "object_pool_interface.h"
#include "cache_refresh.h"
#include "log4_help.h"
#include "system_manager.h"

template <typename T>
class DynamicObjectPool :public IDynamicObjectPool
{
public:
    void Dispose() override;

    template<typename ...Targs>
    T* MallocObject(SystemManager* pSys, Targs... args);

    virtual void Update() override;
    virtual void FreeObject(IComponent* pObj) override;

    virtual void Show() override;

protected:
    std::queue<T*> _free;
    CacheRefresh<T> _objInUse;

#if _DEBUG
    int _totalCall{ 0 };
#endif
};

template <typename T>
void DynamicObjectPool<T>::Dispose()
{
    std::cout << "delete pool. " << typeid(T).name() << std::endl;

    if (_objInUse.Count() > 0)
    {
        std::cout << " type:" << typeid(T).name() << " count:" << _objInUse.Count() << std::endl;
    }

    while (!_free.empty())
    {
        auto obj = _free.front();
        delete obj;
        _free.pop();
    }
}

template <typename T>
template <typename ... Targs>
T* DynamicObjectPool<T>::MallocObject(SystemManager* pSys, Targs... args)
{
    if (_free.empty())
    {
        if (T::IsSingle())
        {
            T* pObj = new T();
            pObj->SetPool(this);
            _free.push(pObj);
        }
        else
        {
            for (int index = 0; index < 50; index++)
            {
                T* pObj = new T();
                pObj->SetPool(this);
                _free.push(pObj);
            }
        }
    }

#if _DEBUG
    _totalCall++;
#endif

    auto pObj = _free.front();
    _free.pop();

    pObj->ResetSN();
    pObj->SetPool(this);
    pObj->SetSystemManager(pSys);
    pObj->Awake(std::forward<Targs>(args)...);

#if LOG_SYSOBJ_OPEN
    LOG_SYSOBJ("*[pool] awake obj. obj sn:" << pObj->GetSN() << " type:" << pObj->GetTypeName() << " thead id:" << std::this_thread::get_id());
#endif

    _objInUse.AddObj(pObj);
    return pObj;
}

template <typename T>
void DynamicObjectPool<T>::Update()
{
    if (_objInUse.CanSwap())
    {
        _objInUse.Swap(&_free);
    }
}

template<typename T>
inline void DynamicObjectPool<T>::FreeObject(IComponent* pObj)
{
    if (pObj->GetSN() == 0)
    {
        LOG_ERROR("free obj sn == 0. type:" << typeid(T).name());
        return;
    }

#if LOG_SYSOBJ_OPEN
    LOG_SYSOBJ("*[pool] free obj. obj sn:" << pObj->GetSN() << " type:" << pObj->GetTypeName() << " thead id:" << std::this_thread::get_id());
#endif

    _objInUse.RemoveObj(pObj->GetSN());
}

template <typename T>
void DynamicObjectPool<T>::Show()
{
    std::stringstream log;
    log << " total:" << std::setw(5) << std::setfill(' ') << _free.size() + _objInUse.Count()
        << "    free:" << std::setw(5) << std::setfill(' ') << _free.size()
        << "    use:" << std::setw(5) << std::setfill(' ') << _objInUse.Count()

#if _DEBUG
        << "    call:" << std::setw(5) << std::setfill(' ') << _totalCall
#endif

        << "    " << typeid(T).name();

    LOG_DEBUG(log.str().c_str());
}
