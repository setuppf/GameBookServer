#pragma once

#include "login_obj.h"
#include <mutex>
#include <map>
#include "libserver/disposable.h"

class LoginObjMgr :public IDisposable
{
public:
	void AddPlayer(SOCKET socket, std::string account, std::string password);
	void RemovePlayer(SOCKET socket);

	LoginObj* GetPlayer(SOCKET socket);
	LoginObj* GetPlayer(std::string account);

	void Dispose() override;

private:
	// 正在验证账号
	// <socket, loginPlayer*>
	std::map<SOCKET, LoginObj*> _players;

	// <account, socket>
	std::map <std::string, SOCKET> _accounts;
};

