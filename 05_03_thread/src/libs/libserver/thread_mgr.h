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

    void Update() override;
    void UpdateCreatePacket();
    void UpdateDispatchPacket();

    bool IsStopAll();
    bool IsDisposeAll();
    void Dispose() override;

    template<class T, typename ...TArgs>
    void CreateComponent(TArgs... args);

    template<class T, typename ...TArgs>
    void CreateComponent(ThreadType iType, TArgs... args);

    // message
    void DispatchPacket(Packet* pPacket);

private:
    template <typename...Args>
    void AnalyseParam(Proto::CreateComponent& proto, int value, Args...args);
    template <typename...Args>
    void AnalyseParam(Proto::CreateComponent& proto, std::string value, Args...args);

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

template<class T, typename ...TArgs>
void ThreadMgr::CreateComponent(TArgs ...args)
{
    CreateComponent<T>(LogicThread, std::forward<TArgs>(args)...);
}

template<class T, typename ...TArgs>
void ThreadMgr::CreateComponent(ThreadType iType, TArgs ...args)
{
    std::lock_guard<std::mutex> guard(_create_lock);

    const std::string className = typeid(T).name();
    if (!ComponentFactory<TArgs...>::GetInstance()->IsRegisted(className))
    {
        RegistToFactory<T, TArgs...>();
    }

    Proto::CreateComponent proto;
    proto.set_thread_type((int)iType);
    proto.set_class_name(className.c_str());
    AnalyseParam(proto, std::forward<TArgs>(args)...);

    auto pCreatePacket = MessageSystemHelp::CreatePacket(Proto::MsgId::MI_CreateComponent, 0);
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
