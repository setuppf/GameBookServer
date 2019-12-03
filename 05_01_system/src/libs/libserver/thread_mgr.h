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

class Packet;

class ThreadMgr :public Singleton<ThreadMgr>, public SystemManager
{
public:
    ThreadMgr();
    void StartAllThread();
    void Update() override;

    bool IsStopAll();
    bool IsDisposeAll();
    void Dispose() override;

    void CreateThread();

    template<class T, typename ...TArgs>
    void CreateComponent(TArgs... args);

    // message
    void DispatchPacket(Packet* pPacket);

private:
private:
    template <typename...Args>
    void AnalyseParam(Proto::CreateComponent& proto, int value, Args...args);
    template <typename...Args>
    void AnalyseParam(Proto::CreateComponent& proto, std::string value, Args...args);

    void AnalyseParam(Proto::CreateComponent& proto)
    {
    }

private:
    std::vector<Thread*> _threads;
    size_t _threadIndex{ 0 };	// 实现线程对象均分

    // 创建组件消息
    std::mutex _create_lock;
    CacheSwap<Packet> _createPackets;
};

template<class T, typename ...TArgs>
inline void ThreadMgr::CreateComponent(TArgs ...args)
{
	std::lock_guard<std::mutex> guard(_create_lock);

	const std::string className = typeid(T).name();
	if (!ComponentFactory<TArgs...>::GetInstance()->IsRegisted(className))
	{
		RegistToFactory<T, TArgs...>();
	}

	Proto::CreateComponent proto;
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
