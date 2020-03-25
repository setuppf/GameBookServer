#include "player_mgr.h"
#include <iostream>


void PlayerMgr::Dispose()
{
	auto iter = _players.begin();
	while (iter != _players.end())
	{
		delete iter->second;
		++iter;
	}

	_players.clear();
}

void PlayerMgr::AddPlayer(SOCKET socket, std::string account, std::string password)
{
	auto iter = _players.find(socket);
	if (iter != _players.end())
	{
		std::cout << "AddPlayer error." << std::endl;
		return;
	}

	_accounts[account] = socket;
	_players[socket] = new Player(socket, account, password);
}

void PlayerMgr::RemovePlayer(SOCKET socket)
{
	auto iter = _players.find(socket);
	if (iter == _players.end())
		return;

	Player* pPlayer = iter->second;

	_players.erase(socket);
	_accounts.erase(pPlayer->GetAccount());

	delete pPlayer;
}

Player* PlayerMgr::GetPlayer(SOCKET socket)
{
	auto iter = _players.find(socket);
	if (iter == _players.end())
		return nullptr;

	return iter->second;
}

Player* PlayerMgr::GetPlayer(std::string account)
{
	auto iter = _accounts.find(account);
	if (iter == _accounts.end())
		return nullptr;

	int socket = iter->second;
	auto iterPlayer = _players.find(socket);
	if (iterPlayer == _players.end())
	{
		_accounts.erase(account);
		return nullptr;
	}

	return iterPlayer->second;
}
