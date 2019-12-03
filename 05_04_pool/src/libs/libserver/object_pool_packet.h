#pragma once
#include "object_pool.h"
#include "packet.h"
#include "singleton.h"

#include <mutex>

class DynamicPacketPool :public DynamicObjectPool<Packet>, public Singleton<DynamicPacketPool>
{
public:
    Packet* MallocPacket(Proto::MsgId msgId, SOCKET socket);
    virtual void Update() override;

private:
    std::mutex _packet_lock;
};
