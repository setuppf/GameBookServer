#include "world_component_gather.h"

#include "libserver/message_system_help.h"
#include "libplayer/player_collector_component.h"
#include "libplayer/world_base.h"

void WorldComponentGather::Awake()
{
    AddTimer(0, 10, true, 1, BindFunP0(this, &WorldComponentGather::SyncWorldInfoToGather));
}

void WorldComponentGather::BackToPool()
{
    
}

void WorldComponentGather::SyncWorldInfoToGather() const
{
    const auto pWorld = GetParent();

    Proto::WorldProxySyncToGather proto;
    proto.set_world_sn(pWorld->GetSN());
    proto.set_world_proxy_sn(pWorld->GetSN());
    proto.set_world_id(dynamic_cast<IWorld*>(pWorld)->GetWorldId());

    const int online = _parent->GetComponent<PlayerCollectorComponent>()->OnlineSize();
    proto.set_online(online);

    MessageSystemHelp::DispatchPacket(Proto::MsgId::MI_WorldProxySyncToGather, proto, nullptr);
}
