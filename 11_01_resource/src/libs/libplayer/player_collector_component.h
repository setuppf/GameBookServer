#pragma once
#include "libserver/system.h"
#include "libserver/component.h"
#include "libserver/socket_object.h"

class Player;

class PlayerCollectorComponent :public Component<PlayerCollectorComponent>, public IAwakeFromPoolSystem<>
{
public:
	void Awake() override;
    void BackToPool() override;

	Player* AddPlayer(NetworkIdentify* pIdentify, std::string account);

	void RemovePlayerBySocket(SOCKET socket);
	void RemovePlayerBySn(uint64 playerSn);

	Player* GetPlayerBySocket(SOCKET socket);
	Player* GetPlayerByAccount(std::string account);
	Player* GetPlayerBySn(uint64 playerSn);

	int OnlineSize() const;
	std::map<SOCKET, Player*>& GetAll();

private:
	// <socket, Player*>
	std::map<SOCKET, Player*> _players;

	// <account, socket>
	std::map <std::string, SOCKET> _accounts;
};

