#include "world_gather.h"

#include "libserver/thread_mgr.h"
#include "libserver/network_locator.h"
#include "libserver/message_system_help.h"
#include "libserver/global.h"
#include "libserver/message_system.h"

void WorldGather::Awake()
{
	AddTimer(0, 10, true, 2, BindFunP0(this, &WorldGather::SyncSpaceInfo));

    auto pMsgSystem = GetSystemManager()->GetMessageSystem();

    pMsgSystem->RegisterFunction(this, Proto::MsgId::MI_WorldSyncToGather, BindFunP1(this, &WorldGather::HandleWorldSyncToGather));
    pMsgSystem->RegisterFunction(this, Proto::MsgId::MI_CmdWorld, BindFunP1(this, &WorldGather::HandleCmdWorld));
}

void WorldGather::BackToPool()
{
	_maps.clear();
}

void WorldGather::SyncSpaceInfo()
{
	Proto::AppInfoSync proto;

	int online = 0;
	auto iter = _maps.begin();
	while (iter != _maps.end())
	{
		online += iter->second;
		++iter;
	}

	const auto pGlobal = Global::GetInstance();
	proto.set_app_id(pGlobal->GetCurAppId());
	proto.set_app_type((int)pGlobal->GetCurAppType());
	proto.set_online(online);

    // 对链接的所有Game发送本进程当前状态
    MessageSystemHelp::SendPacketToAllApp(Proto::MsgId::MI_AppInfoSync, proto, APP_GAME);

    // 发送给appmgr
    MessageSystemHelp::SendPacket(Proto::MsgId::MI_AppInfoSync, proto, APP_APPMGR);
}

void WorldGather::HandleWorldSyncToGather(Packet* pPacket)
{
	auto proto = pPacket->ParseToProto<Proto::WorldSyncToGather>();
	_maps[proto.world_sn()] = proto.online();
}

void WorldGather::HandleCmdWorld(Packet* pPacket)
{
	LOG_DEBUG("**** world gather ****");
	for (auto one : _maps)
	{
		LOG_DEBUG("sn:" << one.first << " online:" << one.second);
	}
}
