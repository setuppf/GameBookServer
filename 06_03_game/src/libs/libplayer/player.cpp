#include "player.h"
#include "player_component.h"

void Player::Awake(NetworkIdentify* pIdentify, std::string account)
{
    _account = account;
    _playerSn = 0;
    _player.Clear();

    if (pIdentify != nullptr)
        _socketKey = pIdentify->GetSocketKey();

    _objKey = { ObjectKeyType::Account, ObjectKeyValue{0, _account} };
}

void Player::BackToPool()
{
    _account = "";
    _name = "";
    _playerSn = 0;
    _player.Clear();

    _socketKey.Clean();
    _objKey.Clean();
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
