#pragma once
#include "libserver/common.h"
#include "libserver/socket_object.h"
#include "libserver/entity.h"
#include "libserver/system.h"

#include "player_component.h"

class Player :public Entity<Player>, public NetworkIdentify, public IAwakeFromPoolSystem<NetworkIdentify*, std::string>
{
public:
	void Awake(NetworkIdentify* pIdentify, std::string account) override;
    void BackToPool() override;

	std::string GetAccount() const;
	std::string GetName() const;
	uint64 GetPlayerSN() const;

	Proto::Player& GetPlayerProto();

	void ParseFromStream(uint64 playerSn, std::stringstream* pOpStream);
	void ParserFromProto(uint64 playerSn, const Proto::Player& proto);
	void SerializeToProto(Proto::Player* pProto) const;

protected:
	std::string _account{ "" };
	std::string _name{ "" };

	uint64 _playerSn{ 0 };
	Proto::Player _player;
};

