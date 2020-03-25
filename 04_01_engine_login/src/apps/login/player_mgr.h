#pragma once

#include "player.h"
#include <mutex>
#include <map>
#include "libserver/disposable.h"

class PlayerMgr :public IDisposable
{
public:
	void AddPlayer(SOCKET socket, std::string account, std::string password);
	void RemovePlayer(SOCKET socket);

	Player* GetPlayer(SOCKET socket);
	Player* GetPlayer(std::string account);

	void Dispose() override;

private:
	// 正在验证账号
	// <socket, loginPlayer*>
	std::map<SOCKET, Player*> _players;

	// <account, socket>
	std::map <std::string, SOCKET> _accounts;
};

