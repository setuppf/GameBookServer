#pragma once

#include <map>
#include "log4_help.h"

#define DynamicStateCreate(classname, enumType) \
    static void* CreateState() { return new classname; } \
    RobotStateType GetState( ) override { return enumType; }

#define DynamicStateBind(classname) \
    reinterpret_cast<CreateIstancePt>( &( classname::CreateState ) )

// T 为管理状态的类
template<typename enumType, class T>
class StateTemplate {
public:
    StateTemplate() {
        _pParentObj = nullptr;
    }

    void SetParentObj(T* pObj) {
        _pParentObj = pObj;
    }

    virtual ~StateTemplate() { }
    virtual enumType GetState() = 0;
    virtual enumType Update() = 0;

    virtual void EnterState() = 0;
    virtual void LeaveState() = 0;

protected:
    T* _pParentObj{ nullptr };
};

template<typename enumType, class StateClass, class T>
class StateTemplateMgr {
public:
    virtual ~StateTemplateMgr() {
        if (_pState != nullptr) {
            delete _pState;
        }
    }

    void InitStateTemplateMgr(enumType defaultState) {
        _defaultState = defaultState;
        RegisterState();
    }

    void ChangeState(enumType stateType) {

        // 状态相同，不处理
        if (_pState != nullptr && _pState->GetState() == stateType)
        {
            LOG_ERROR("ChangeState: same state type:" << GetRobotStateTypeShortName(stateType));
            return;
        }

        StateClass* pNewState = CreateStateObj(stateType);
        if (pNewState == nullptr) {
            //LOG_ERROR( "ChangeState:" << stateType << " == nullptr" );
            return;
        }

        if (pNewState != nullptr) {
            if (_pState != nullptr) {
                _pState->LeaveState();
                delete _pState;
            }

            _pState = pNewState;
            _pState->EnterState();
        }
    }

    void UpdateState() {
        if (_pState == nullptr) {
            ChangeState(_defaultState);
        }
        enumType curState = _pState->Update();
        if (curState != _pState->GetState()) {
            ChangeState(curState);
        }
    }

protected:
    virtual void RegisterState() = 0;

    ///////////////////////////////////////////////////////////////////////////////////
    // 创建状态类
public:
    typedef StateClass* (*CreateIstancePt)();

    StateClass* CreateStateObj(enumType enumValue) {
        auto iter = _dynCreateMap.find(enumValue);
        if (iter == _dynCreateMap.end())
            return nullptr;

        CreateIstancePt np = iter->second;
        StateClass* pState = np();
        pState->SetParentObj(static_cast<T*>(this));
        return pState;
    }

    void RegisterStateClass(enumType enumValue, CreateIstancePt np) {
        _dynCreateMap[enumValue] = np;
    }

    ///////////////////////////////////////////////////////////////////////////////////

protected:
    std::map<enumType, CreateIstancePt> _dynCreateMap;
    StateClass* _pState{ nullptr };
    enumType _defaultState;
};
