#include "world_proxy_help.h"

#include "libserver/message_system_help.h"

#include "libplayer/player.h"
#include "libserver/thread_mgr.h"

void WorldProxyHelp::Teleport(Player* pPlayer, const uint64 lastWorldSn, const uint64 targetWorldSn)
{
    const uint64 playerSn = pPlayer->GetPlayerSN();

    Proto::Teleport proto;
    proto.set_last_world_sn(lastWorldSn);
    proto.set_account(pPlayer->GetAccount().c_str());
    proto.set_player_sn(playerSn);
    pPlayer->SerializeToProto(proto.mutable_player());

    NetIdentify netIdentify;
    netIdentify.GetSocketKey()->CopyFrom(pPlayer->GetSocketKey());
    netIdentify.GetTagKey()->AddTag(TagType::Entity, targetWorldSn);
    MessageSystemHelp::DispatchPacket(Proto::MsgId::MI_Teleport, proto, &netIdentify);
}
