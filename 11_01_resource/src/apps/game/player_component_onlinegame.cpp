#include "player_component_onlinegame.h"
#include "libserver/redis_constants.h"

#include "libplayer/player.h"
#include "libserver/entity.h"
#include "libserver/message_system_help.h"

void PlayerComponentOnlineInGame::Awake(const std::string account, int version)
{
	_account = account;
	_onlineVersion = version;
	_isReadFromeDB = false;
	AddTimer(0, (int)(RedisKeyAccountOnlineGameTimeout*0.5), true, 0, BindFunP0(this, &PlayerComponentOnlineInGame::SetOnlineFlag));
}

void PlayerComponentOnlineInGame::Awake(const std::string account)
{
	_account = account;
	_onlineVersion = 0;
	_isReadFromeDB = true;

	Player* pPlayer = static_cast<Player*>(_parent);
	ParserFromProto(pPlayer->GetPlayerProto());
	_onlineVersion++;

    AddTimer(0, (int)(RedisKeyAccountOnlineGameTimeout*0.5), true, 0, BindFunP0(this, &PlayerComponentOnlineInGame::SetOnlineFlag));
}

void PlayerComponentOnlineInGame::BackToPool()
{
	Proto::PlayerDeleteOnlineToRedis protoSync;
	protoSync.set_account(_account.c_str());
	protoSync.set_version(_onlineVersion);
	MessageSystemHelp::DispatchPacket(Proto::MsgId::MI_PlayerDeleteOnlineToRedis, protoSync, nullptr);
}

void PlayerComponentOnlineInGame::SetOnlineFlag() const
{
	// 设置在线标志
	Proto::PlayerSyncOnlineToRedis protoSync;
	protoSync.set_account(_account.c_str());
	protoSync.set_version(_onlineVersion);
	MessageSystemHelp::DispatchPacket(Proto::MsgId::MI_PlayerSyncOnlineToRedis, protoSync,  nullptr);
}

void PlayerComponentOnlineInGame::ParserFromProto(const Proto::Player& proto)
{
	if (!_isReadFromeDB)
		return;

	const auto version = proto.misc().online_version();
	if (_onlineVersion < version)
		_onlineVersion = version;
}

void PlayerComponentOnlineInGame::SerializeToProto(Proto::Player* pProto)
{
	pProto->mutable_misc()->set_online_version(_onlineVersion);
}
