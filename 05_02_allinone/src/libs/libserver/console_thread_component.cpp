#include "console_thread_component.h"

#include "message_component.h"
#include "create_component.h"
#include "update_component.h"
#include "entity_system.h"

#include <thread>
#include <iostream>

void ConsoleThreadComponent::AwakeFromPool()
{
    auto pMsgCallBack = new MessageCallBackFunction();
    AddComponent<MessageComponent>(pMsgCallBack);
    pMsgCallBack->RegisterFunction(Proto::MsgId::MI_CmdShowThreadEntites, BindFunP1(this, &ConsoleThreadComponent::HandleCmdShowThreadEntites));
}

void ConsoleThreadComponent::BackToPool()
{

}

void ConsoleThreadComponent::HandleCmdShowThreadEntites(Packet* pPacket)
{
    std::list<uint64> excludes;
    excludes.push_back(typeid(MessageComponent).hash_code());
    excludes.push_back(typeid(CreateComponentC).hash_code());
    excludes.push_back(typeid(UpdateComponent).hash_code());
    excludes.push_back(typeid(ConsoleThreadComponent).hash_code());

    std::stringstream log;
    log << "*************************** " << "\n";
    log << " thread id:" << std::this_thread::get_id() << "\n";
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
        log << "\t" << pCollect->GetClassType().c_str() << " count:" << size << "\n";
    }

    log << " total count:" << total << "\n";
    LOG_DEBUG(log.str().c_str());
}
