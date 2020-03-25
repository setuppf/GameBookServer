#include "object_pool_packet.h"
#include "component_help.h"
#include "socket_object.h"

Packet* DynamicPacketPool::MallocPacket(Proto::MsgId msgId, NetIdentify* pIdentify)
{
    std::lock_guard<std::mutex> guard(_packet_lock);

    const auto pPacket = DynamicObjectPool<Packet>::MallocObject(nullptr, nullptr, 0, msgId, pIdentify);

#ifdef LOG_TRACE_COMPONENT_OPEN
    if (pIdentify != nullptr)
    {
        const google::protobuf::EnumDescriptor* descriptor = Proto::MsgId_descriptor();
        const auto name = descriptor->FindValueByNumber(msgId)->name();
        std::stringstream os;
        os << "malloc.  "
            << " sn:" << pPacket->GetSN()
            << " msgId:" << name.c_str()
            << " " << dynamic_cast<NetIdentify*>(pPacket);
        ComponentHelp::GetTraceComponent()->Trace(TraceType::Packet, pIdentify->GetSocketKey()->Socket, os.str());
    }
#endif

    return pPacket;
}

#ifdef LOG_TRACE_COMPONENT_OPEN
inline void TraceFree(Packet* pPacket)
{
    const google::protobuf::EnumDescriptor* descriptor = Proto::MsgId_descriptor();
    const auto name = descriptor->FindValueByNumber(pPacket->GetMsgId())->name();
    std::stringstream os;
    os << "free.    "
        << " sn:" << pPacket->GetSN()
        << " msgId:" << name.c_str()
        << " " << dynamic_cast<NetIdentify*>(pPacket);
    ComponentHelp::GetTraceComponent()->Trace(TraceType::Packet, pPacket->GetSocketKey()->Socket, os.str());
}
#endif

void DynamicPacketPool::Update()
{
    std::lock_guard<std::mutex> guard(_packet_lock);

    DynamicObjectPool<Packet>::Update();

    // 是否需要释放
    auto lists = _objInUse.GetReaderCache();
    for (auto iter = lists->begin(); iter != lists->end(); ++iter)
    {
        auto pPacket = iter->second;
        if (pPacket->CanBack2Pool())
        {

#ifdef LOG_TRACE_COMPONENT_OPEN
            TraceFree(pPacket);
#endif

            DynamicObjectPool<Packet>::FreeObject(pPacket);
        }
    }
}

void DynamicPacketPool::FreeObject(IComponent* pObj)
{
    std::lock_guard<std::mutex> guard(_packet_lock);

#ifdef LOG_TRACE_COMPONENT_OPEN
    TraceFree(dynamic_cast<Packet*>(pObj));
#endif

    DynamicObjectPool<Packet>::FreeObject(pObj);
}

void DynamicPacketPool::Show()
{
    std::lock_guard<std::mutex> guard(_packet_lock);

    std::map<int, int> statData;
    auto lists = _objInUse.GetReaderCache();
    for (auto iter = lists->begin(); iter != lists->end(); ++iter)
    {
        auto pPacket = iter->second;
        int msgId = pPacket->GetMsgId();
        auto iterMsgId = statData.find(msgId);
        if (iterMsgId == statData.end()) {
            statData.insert(std::make_pair(msgId, 0));
        }

        statData[msgId]++;
    }

    const google::protobuf::EnumDescriptor* descriptor = Proto::MsgId_descriptor();
    for (auto iter = statData.begin(); iter != statData.end(); ++iter)
    {
        auto name = descriptor->FindValueByNumber(iter->first)->name();
        LOG_WARN(" msgId:" << name.c_str() << " " << iter->second);
    }

    // 分析一下正在使用的Packet的协议号
    DynamicObjectPool<Packet>::Show();
}
