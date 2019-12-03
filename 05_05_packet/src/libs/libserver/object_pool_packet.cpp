#include "object_pool_packet.h"

Packet* DynamicPacketPool::MallocPacket(Proto::MsgId msgId, SOCKET socket)
{
    std::lock_guard<std::mutex> guard(_packet_lock);

    return DynamicObjectPool<Packet>::MallocObject(nullptr, msgId, socket);
}

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
            DynamicObjectPool<Packet>::FreeObject(pPacket);
        }
    }
}

void DynamicPacketPool::FreeObject(IComponent* pObj)
{
    std::lock_guard<std::mutex> guard(_packet_lock);
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
