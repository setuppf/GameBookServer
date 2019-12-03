#include "player_component_onlineinlogin.h"
#include "libserver/redis_constants.h"

#include "libplayer/player.h"
#include "libserver/message_system_help.h"

void PlayerComponentOnlineInLogin::Awake(std::string account)
{
	_account = account;
	AddTimer(0, (int)(RedisKeyAccountOnlineLoginTimeout*0.5), true, 0, BindFunP0(this, &PlayerComponentOnlineInLogin::SetOnlineFlag));
}

void PlayerComponentOnlineInLogin::BackToPool()
{
	Proto::AccountDeleteOnlineToRedis protoSync;
	protoSync.set_account(_account.c_str());
	MessageSystemHelp::DispatchPacket(Proto::MsgId::MI_AccountDeleteOnlineToRedis, protoSync, nullptr);
}

void PlayerComponentOnlineInLogin::SetOnlineFlag() const
{
	// 设置在线标志
	Proto::AccountSyncOnlineToRedis protoSync;
	protoSync.set_account(_account.c_str());
	MessageSystemHelp::DispatchPacket(Proto::MsgId::MI_AccountSyncOnlineToRedis, protoSync, nullptr);
}
