#include "world.h"
#include "libplayer/player_collector_component.h"
#include "libserver/message_system_help.h"

void World::Awake(int worldId)
{
    LOG_DEBUG("create world. id:" << worldId);

    _worldId = worldId;

    AddComponent<PlayerCollectorComponent>();

    AddTimer(0, 10, false, 0, BindFunP0(this, &World::SyncWorldToGather));

    //auto pMsgSystem = GetSystemManager()->GetMessageSystem();    
}

void World::BackToPool()
{
}

void World::SyncWorldToGather()
{
    Proto::WorldSyncToGather proto;
    proto.set_world_sn(GetSN());
    proto.set_world_id(GetWorldId());

    const int online = GetComponent<PlayerCollectorComponent>()->OnlineSize();
    proto.set_online(online);

    MessageSystemHelp::DispatchPacket(Proto::MsgId::MI_WorldSyncToGather, proto, nullptr);
}
