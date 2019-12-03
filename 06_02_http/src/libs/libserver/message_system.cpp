#include "message_system.h"
#include "system_manager.h"
#include "packet.h"
#include "message_component.h"
#include "entity_system.h"
#include "component.h"
#include "object_pool_packet.h"
#include "component_help.h"

MessageSystem::MessageSystem(SystemManager* pMgr)
{
    _systemMgr = pMgr;
}

void MessageSystem::Dispose()
{

}

void MessageSystem::AddPacketToList(Packet* pPacket)
{
    std::lock_guard<std::mutex> guard(_packet_lock);
    _cachePackets.GetWriterCache()->emplace_back(pPacket);

    // 进入时 Ref +1
    pPacket->AddRef();
}

void MessageSystem::Update(EntitySystem* pEntities)
{
    _packet_lock.lock();
    if (_cachePackets.CanSwap())
    {
        _cachePackets.Swap();
    }
    _packet_lock.unlock();

    if (_cachePackets.GetReaderCache()->size() == 0)
        return;

    auto pCollections = pEntities->GetComponentCollections<MessageComponent>();
    if (pCollections == nullptr)
    {
        _cachePackets.GetReaderCache()->clear();
        return;
    }

    pCollections->Swap();

    auto lists = pCollections->GetAll();
    auto packetLists = _cachePackets.GetReaderCache();
    for (auto iter = packetLists->begin(); iter != packetLists->end(); ++iter)
    {
        auto pPacket = (*iter);
        Process(pPacket, lists);

        // 离开时 Ref - 1
        pPacket->RemoveRef();
    }

    _cachePackets.GetReaderCache()->clear();
}

void MessageSystem::Process(Packet* pPacket, std::map<uint64, IComponent*>& lists)
{
    for (auto iter = lists.begin(); iter != lists.end(); ++iter)
    {
        MessageComponent* pMsgComponent = static_cast<MessageComponent*>(iter->second);
        if (pMsgComponent->IsFollowMsgId(pPacket))
        {
#ifdef LOG_TRACE_COMPONENT_OPEN
            const google::protobuf::EnumDescriptor* descriptor = Proto::MsgId_descriptor();
            const auto name = descriptor->FindValueByNumber(pPacket->GetMsgId())->name();

            auto pParent = pMsgComponent->GetParent();
            const auto traceMsg = std::string("process. ")
                .append(" sn:").append(std::to_string(pPacket->GetSN()))
                .append(" msgId:").append(name)
                .append(" process by:").append(pParent->GetTypeName());                
            ComponentHelp::GetTraceComponent()->Trace(TraceType::Packet, pPacket->GetSocketKey().Socket, traceMsg);
#endif

            pMsgComponent->ProcessPacket(pPacket);
        }
    }
}
