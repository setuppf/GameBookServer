#pragma once

#include <map>

#include "common.h"
#include "system.h"
#include "cache_swap.h"

class IComponent;
class SystemManager;
class Packet;
class EntitySystem;

class MessageSystem :virtual public ISystem
{
public:
    MessageSystem(SystemManager* pMgr);
    void Dispose() override;

    void Update(EntitySystem* pEntities) override;
    void AddPacketToList(Packet* pPacket);

private:
    static void Process(Packet* pPacket, std::map<uint64, IComponent*>& lists);

private:
    // 本线程中的所有待处理包
    std::mutex _packet_lock;
    CacheSwap<Packet> _cachePackets;

    SystemManager* _systemMgr{ nullptr };
};