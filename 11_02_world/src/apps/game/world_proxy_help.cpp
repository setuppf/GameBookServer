#include "world_proxy_help.h"

#include "libserver/message_system_help.h"
#include "libplayer/player.h"
#include "libserver/packet.h"
#include "world_proxy_locator.h"
#include "libserver/thread_mgr.h"

void WorldProxyHelp::Teleport(Player* pPlayer, const uint64 oriWorldSn, const uint64 targetWorldSn)
{
	//const uint64 playerSn = pPlayer->GetPlayerSN();

	//Proto::Teleport proto;
	//proto.set_origina_world_sn(oriWorldSn);
	//proto.set_account(pPlayer->GetAccount().c_str());
	//proto.set_player_sn(playerSn);
	//pPlayer->SerializeToProto(proto.mutable_player());

	//// teleport传给 worldproxy，包括玩家socket。得到一个teleportrs
	//auto pWorldLoactor = ThreadMgr::GetInstance()->GetEntitySystem()->GetComponent<WorldProxyLocator>();
	//WorldProxyBriefData briefData;
	//if (!pWorldLoactor->GetWorldProxySn(targetWorldSn, &briefData))
	//{
	//	LOG_WARN("can'f find targetWorldSn");
	//}

	//MessageSystemHelp::DispatchPacket(Proto::MsgId::MI_Teleport, pPlayer->GetSocket(), proto, targetWorldSn, playerSn);
}
