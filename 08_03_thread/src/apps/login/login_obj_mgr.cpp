#include "login_obj_mgr.h"
#include <iostream>

void LoginObjMgr::BackToPool()
{
	auto iter = _players.begin();
	while (iter != _players.end())
	{
		delete iter->second;
		++iter;
	}

	_players.clear();
    _accounts.clear();
}

void LoginObjMgr::AddPlayer(SOCKET socket, std::string account, std::string password)
{
	auto iter = _players.find(socket);
	if (iter != _players.end())
	{
		std::cout << "AddPlayer error." << std::endl;
		return;
	}

	_accounts[account] = socket;
	_players[socket] = new LoginObj(socket, account, password);
}

void LoginObjMgr::RemovePlayer(SOCKET socket)
{
	auto iter = _players.find(socket);
	if (iter == _players.end())
		return;

	LoginObj* pPlayer = iter->second;

	_players.erase(socket);
	_accounts.erase(pPlayer->GetAccount());

	delete pPlayer;
}

LoginObj* LoginObjMgr::GetPlayer(SOCKET socket)
{
	auto iter = _players.find(socket);
	if (iter == _players.end())
		return nullptr;

	return iter->second;
}

LoginObj* LoginObjMgr::GetPlayer(std::string account)
{
	auto iter = _accounts.find(account);
	if (iter == _accounts.end())
		return nullptr;

	SOCKET socket = iter->second;
	auto iterPlayer = _players.find(socket);
	if (iterPlayer == _players.end())
	{
		_accounts.erase(account);
		return nullptr;
	}

	return iterPlayer->second;
}
