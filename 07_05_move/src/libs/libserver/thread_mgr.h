#pragma once

#include <mutex>
#include <vector>

#include "common.h"
#include "thread.h"
#include "cache_swap.h"
#include "singleton.h"
#include "entity_system.h"
#include "component_factory.h"
#include "regist_to_factory.h"
#include "message_system_help.h"
#include "thread_collector.h"
#include "thread_type.h"

class ThreadMgr :public Singleton<ThreadMgr>, public SystemManager
{
public:
    ThreadMgr();
    void InitializeThread();
    void CreateThread(ThreadType iType, int num);

    void InitializeGlobalComponent(APP_TYPE ppType, int appId);
    void Update() override;
    void UpdateCreatePacket();
    void UpdateDispatchPacket();

    bool IsStopAll();
    void DestroyThread();
    bool IsDestroyAll();

    void Dispose() override;

    template<class T, typename ...TArgs>
    void CreateSystem(TArgs... args);

    template<class T, typename ...TArgs>
    void CreateComponent(TArgs... args);

    template<class T, typename ...TArgs>
    void CreateComponentWithSn(uint64 sn, TArgs... args);

    template<class T, typename ...TArgs>
    void CreateComponent(bool isToAllThead, TArgs... args);

    template<class T, typename ...TArgs>
    void CreateComponent(ThreadType iType, bool isToAllThead, TArgs... args);

    // message
    void DispatchPacket(Packet* pPacket);

private:

    template<class T, typename ...TArgs>
    void CreateComponentWithSn(ThreadType iType, uint64 sn, bool isToAllThead, TArgs... args);

    template <typename...Args>
    void AnalyseParam(Proto::CreateComponent& proto, int value, Args...args);
    template <typename...Args>
    void AnalyseParam(Proto::CreateComponent& proto, std::string value, Args...args);
    template <typename...Args>
    void AnalyseParam(Proto::CreateComponent& proto, uint64 value, Args...args);

    void AnalyseParam(Proto::CreateComponent& proto)
    {
    }

private:
    std::map<ThreadType, ThreadCollector*> _threads;

    // 创建组件消息
    std::mutex _create_lock;
    CacheSwap<Packet> _createPackets;

    // 创建组件消息
    std::mutex _packet_lock;
    CacheSwap<Packet> _packets;
};

template <class T, typename ... TArgs>
void ThreadMgr::CreateSystem(TArgs... args)
{
    const std::string className = typeid(T).name();
    if (!ComponentFactory<TArgs...>::GetInstance()->IsRegisted(className))
    {
        RegistObject<T, TArgs...>();
    }

    Proto::CreateSystem proto;
    proto.set_system_name(className.c_str());
    proto.set_thread_type((int)LogicThread);

    auto pCreatePacket = MessageSystemHelp::CreatePacket(Proto::MsgId::MI_CreateSystem, nullptr);
    pCreatePacket->SerializeToBuffer(proto);
    DispatchPacket(pCreatePacket);
}

template<class T, typename ...TArgs>
void ThreadMgr::CreateComponent(TArgs ...args)
{
    CreateComponent<T>(LogicThread, false, std::forward<TArgs>(args)...);
}

template <class T, typename ... TArgs>
void ThreadMgr::CreateComponentWithSn(uint64 sn, TArgs... args)
{
    CreateComponentWithSn<T>(LogicThread, sn, false, std::forward<TArgs>(args)...);
}

template<class T, typename ...TArgs>
void ThreadMgr::CreateComponent(bool isToAllThead, TArgs ...args)
{
    CreateComponent<T>(LogicThread, isToAllThead, std::forward<TArgs>(args)...);
}

template<class T, typename ...TArgs>
void ThreadMgr::CreateComponent(ThreadType iType, bool isToAllThead, TArgs ...args)
{
    CreateComponentWithSn<T>(iType, static_cast<uint64>(0), isToAllThead, std::forward<TArgs>(args)...);
}

template <class T, typename ... TArgs>
void ThreadMgr::CreateComponentWithSn(ThreadType iType, uint64 sn, bool isToAllThead, TArgs... args)
{
    std::lock_guard<std::mutex> guard(_create_lock);

    const std::string className = typeid(T).name();
    if (!ComponentFactory<TArgs...>::GetInstance()->IsRegisted(className))
    {
        RegistToFactory<T, TArgs...>();
    }

    Proto::CreateComponent proto;
    proto.set_thread_type((int)iType);
    proto.set_sn(sn);
    proto.set_class_name(className.c_str());
    proto.set_is_to_all_thread(isToAllThead);
    AnalyseParam(proto, std::forward<TArgs>(args)...);

    auto pCreatePacket = MessageSystemHelp::CreatePacket(Proto::MsgId::MI_CreateComponent, nullptr);
    pCreatePacket->SerializeToBuffer(proto);
    _createPackets.GetWriterCache()->emplace_back(pCreatePacket);
}

template<typename ... Args>
void ThreadMgr::AnalyseParam(Proto::CreateComponent& proto, int value, Args... args)
{
    auto pProtoParam = proto.mutable_params()->Add();
    pProtoParam->set_type(Proto::CreateComponentParam::Int);
    pProtoParam->set_int_param(value);
    AnalyseParam(proto, std::forward<Args>(args)...);
}

template<typename ... Args>
void ThreadMgr::AnalyseParam(Proto::CreateComponent& proto, std::string value, Args... args)
{
    auto pProtoParam = proto.mutable_params()->Add();
    pProtoParam->set_type(Proto::CreateComponentParam::String);
    pProtoParam->set_string_param(value.c_str());
    AnalyseParam(proto, std::forward<Args>(args)...);
}

template <typename ... Args>
void ThreadMgr::AnalyseParam(Proto::CreateComponent& proto, uint64 value, Args... args)
{
    auto pProtoParam = proto.mutable_params()->Add();
    pProtoParam->set_type(Proto::CreateComponentParam::UInt64);
    pProtoParam->set_uint64_param(value);
    AnalyseParam(proto, std::forward<Args>(args)...);
}
