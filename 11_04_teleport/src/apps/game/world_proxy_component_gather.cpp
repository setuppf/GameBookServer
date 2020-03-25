#include "world_proxy_component_gather.h"

#include "libserver/message_system_help.h"
#include "libplayer/player_collector_component.h"
#include "libplayer/world_base.h"

void WorldProxyComponentGather::Awake()
{
    const auto pWorld = GetParent();
    _worldSn = pWorld->GetSN();
    _worldId = dynamic_cast<IWorld*>(_parent)->GetWorldId();
    AddTimer(0, 2, false, 0, BindFunP0(this, &WorldProxyComponentGather::SyncWorldInfoToGather));
}

void WorldProxyComponentGather::BackToPool()
{
    Proto::WorldProxySyncToGather proto;
    proto.set_world_sn(_worldSn);
    proto.set_is_remove(true);

    MessageSystemHelp::DispatchPacket(Proto::MsgId::MI_WorldProxySyncToGather, proto, nullptr);
}

void WorldProxyComponentGather::SyncWorldInfoToGather()
{
    Proto::WorldProxySyncToGather proto;
    proto.set_world_sn(_worldSn);
    proto.set_is_remove(false);
    proto.set_world_id(_worldId);

    const int online = _parent->GetComponent<PlayerCollectorComponent>()->OnlineSize();
    proto.set_online(online);

    MessageSystemHelp::DispatchPacket(Proto::MsgId::MI_WorldProxySyncToGather, proto, nullptr);
}
