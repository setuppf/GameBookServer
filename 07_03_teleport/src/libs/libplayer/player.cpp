#include "player.h"
#include "player_component.h"
#include "libserver/message_system_help.h"

void Player::Awake(NetIdentify* pIdentify, std::string account)
{
    _account = account;
    _playerSn = 0;
    _player.Clear();

    if (pIdentify != nullptr)
        _socketKey.CopyFrom(pIdentify->GetSocketKey());

    _tagKey.Clear();
    _tagKey.AddTag(TagType::Account, _account);

    // 登录成功，修改网络底层的标识
    MessageSystemHelp::DispatchPacket(Proto::MsgId::MI_NetworkListenKey, this);
}

void Player::Awake(NetIdentify* pIdentify, uint64 playerSn, uint64 worldSn)
{
    _account = "";
    _playerSn = playerSn;
    _player.Clear();

    if (pIdentify != nullptr)
        _socketKey.CopyFrom(pIdentify->GetSocketKey());

    _tagKey.Clear();
    _tagKey.AddTag(TagType::Player, playerSn);
    _tagKey.AddTag(TagType::Entity, worldSn);

    // space进程调用，但Space不需要修改网络标识
    // 登录成功，修改网络底层的标识
    //MessageSystemHelp::DispatchPacket(Proto::MsgId::MI_NetworkListenKey, this);
}

void Player::BackToPool()
{
    _account = "";
    _name = "";
    _playerSn = 0;
    _player.Clear();

    _socketKey.Clear();
    _tagKey.Clear();
}

std::string Player::GetAccount() const
{
	return _account;
}

std::string Player::GetName() const
{
	return _name;
}

uint64 Player::GetPlayerSN() const
{
	return _playerSn;
}

Proto::Player& Player::GetPlayerProto()
{
	return _player;
}

void Player::ParseFromStream(const uint64 playerSn, std::stringstream* pOpStream)
{
	_playerSn = playerSn;
	_player.ParseFromIstream(pOpStream);
}

void Player::ParserFromProto(const uint64 playerSn, const Proto::Player& proto)
{
	_playerSn = playerSn;
	_player.CopyFrom(proto);
	_name = _player.name();

	// 内存中修改数据
	for (auto pair : _components)
	{
		auto pPlayerComponent = dynamic_cast<PlayerComponent*>(pair.second);
		if (pPlayerComponent == nullptr)
			continue;

		pPlayerComponent->ParserFromProto(proto);
	}
}

void Player::SerializeToProto(Proto::Player* pProto) const
{
	// 基础数据
	pProto->CopyFrom(_player);

	// 内存中修改数据
	for (auto pair : _components)
	{
		auto pPlayerComponent = dynamic_cast<PlayerComponent*>(pair.second);
		if (pPlayerComponent == nullptr)
			continue;

		pPlayerComponent->SerializeToProto(pProto);
	}
}
