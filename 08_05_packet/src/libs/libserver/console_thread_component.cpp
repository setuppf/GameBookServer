#include "console_thread_component.h"

#include "message_component.h"
#include "create_component.h"
#include "update_component.h"
#include "entity_system.h"

#include <thread>

std::mutex ConsoleThreadComponent::_show_lock;

void ConsoleThreadComponent::Awake(ThreadType iType)
{
    _threadType = iType;

    auto pMsgCallBack = new MessageCallBackFunction();
    AddComponent<MessageComponent>(pMsgCallBack);
    pMsgCallBack->RegisterFunction(Proto::MsgId::MI_CmdThread, BindFunP1(this, &ConsoleThreadComponent::HandleCmdThread));
}

void ConsoleThreadComponent::BackToPool()
{

}

void ConsoleThreadComponent::HandleCmdThread(Packet* pPacket)
{
    auto cmdProto = pPacket->ParseToProto<Proto::CmdThread>();
    auto cmdType = cmdProto.cmd_type();
    if (cmdType == Proto::CmdThread_CmdType_Entity)
        HandleCmdThreadEntites(pPacket);
    else
        HandleCmdThreadPool(pPacket);
}

void ConsoleThreadComponent::HandleCmdThreadPool(Packet* pPacket)
{
    std::lock_guard<std::mutex> guard(_show_lock);

    LOG_DEBUG("------------------------------------");
    LOG_DEBUG(" thread id:" << std::this_thread::get_id());
    LOG_DEBUG(" thread type:" << GetThreadTypeName(_threadType));

    auto pPool = GetSystemManager()->GetPoolCollector();
    pPool->Show();
}

void ConsoleThreadComponent::HandleCmdThreadEntites(Packet* pPacket)
{
    std::lock_guard<std::mutex> guard(_show_lock);

    std::list<uint64> excludes;
    excludes.push_back(typeid(MessageComponent).hash_code());
    excludes.push_back(typeid(CreateComponentC).hash_code());
    excludes.push_back(typeid(UpdateComponent).hash_code());
    excludes.push_back(typeid(ConsoleThreadComponent).hash_code());

    LOG_DEBUG("------------------------------------");
    LOG_DEBUG(" thread id:" << std::this_thread::get_id());
    LOG_DEBUG(" thread type:" << GetThreadTypeName(_threadType));

    auto collects = GetSystemManager()->GetEntitySystem()->_objSystems;
    int total = 0;
    for (const auto one : collects)
    {
        if (std::find(excludes.begin(), excludes.end(), one.first) != excludes.end())
            continue;

        auto pCollect = one.second;
        const auto size = pCollect->GetAll().size();
        if (size <= 0)
            continue;

        total += size;
        LOG_DEBUG("\t" << pCollect->GetClassType().c_str() << " count:" << size);
    }

    LOG_DEBUG(" total count:" << total);
}
