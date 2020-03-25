#include "space_sync_handler.h"

#include "libserver/thread_mgr.h"
#include "libserver/message_system.h"

void SpaceSyncHandler::Awake()
{
    // message
    auto pMsgSystem = GetSystemManager()->GetMessageSystem();
    pMsgSystem->RegisterFunction(this, Proto::MsgId::MI_AppInfoSync, BindFunP1(this, &SpaceSyncHandler::HandleAppInfoSync));
    pMsgSystem->RegisterFunction(this, Proto::MsgId::MI_CmdApp, BindFunP1(this, &SpaceSyncHandler::HandleCmdApp));
    pMsgSystem->RegisterFunction(this, Proto::MsgId::MI_NetworkDisconnect, BindFunP1(this, &SpaceSyncHandler::HandleNetworkDisconnect));
}

void SpaceSyncHandler::BackToPool()
{
    _apps.clear();
}

bool SpaceSyncHandler::GetSpaceApp(AppInfo* pInfo)
{
	return GetOneApp(APP_SPACE, pInfo);
}

void SpaceSyncHandler::HandleAppInfoSync(Packet* pPacket)
{
    AppInfoSyncHandle(pPacket);
}
