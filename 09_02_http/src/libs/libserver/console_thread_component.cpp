#include "console_thread_component.h"

#include "message_component.h"
#include "create_component.h"
#include "update_component.h"
#include "entity_system.h"

#include <thread>
#include "network_listen.h"
#include "util_string.h"
#include "timer_component.h"

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
    else if (cmdType == Proto::CmdThread_CmdType_Connect)
        HandleCmdThreadConnect(pPacket);
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

void ConsoleThreadComponent::HandleCmdThreadConnect(Packet* pPacket)
{
    std::lock_guard<std::mutex> guard(_show_lock);
    const auto pNetworkListen = GetSystemManager()->GetEntitySystem()->GetComponent<NetworkListen>();
    if (pNetworkListen == nullptr)
        return;

    LOG_DEBUG("------------------------------------");
    LOG_DEBUG(" network type:" << GetNetworkTypeName(pNetworkListen->GetNetworkType()));
    pNetworkListen->CmdShow();
}

void ConsoleThreadComponent::HandleCmdThreadEntites(Packet* pPacket)
{
    std::lock_guard<std::mutex> guard(_show_lock);

    std::list<uint64> excludes;
    excludes.push_back(typeid(MessageComponent).hash_code());
    excludes.push_back(typeid(CreateComponentC).hash_code());
    excludes.push_back(typeid(UpdateComponent).hash_code());
    excludes.push_back(typeid(ConsoleThreadComponent).hash_code());
    excludes.push_back(typeid(TimerComponent).hash_code());

    LOG_DEBUG("------------------------------------");
    LOG_DEBUG(" thread id:" << std::this_thread::get_id());
    LOG_DEBUG(" thread type:" << GetThreadTypeName(_threadType));

    NetworkListen* pListen = nullptr;   

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

        std::stringstream log;
        log << " count:" << std::setw(5) << std::setfill(' ') << size << "    " << pCollect->GetClassType().c_str();
        LOG_DEBUG(log.str().c_str());

        if (strutil::stricmp(typeid(NetworkListen).name(), pCollect->GetClassType().c_str()) == 0)
        {
            pListen = dynamic_cast<NetworkListen*>(pCollect->Get());
        }
    }

    if (pListen != nullptr)
    {
        LOG_COLOR(LogColorGreen, "  network type:" << GetNetworkTypeName(pListen->GetNetworkType()));
        pListen->CmdShow();
    }

    LOG_DEBUG(" total count:" << total);
}
