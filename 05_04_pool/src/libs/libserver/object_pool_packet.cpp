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

}
