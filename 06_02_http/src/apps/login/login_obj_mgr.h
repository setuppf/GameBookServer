#pragma once

#include "login_obj.h"
#include <map>
#include "libserver/component.h"

class LoginObjMgr :public Component<LoginObjMgr>
{
public:
    LoginObj* AddPlayer(NetworkIdentify* pIdentify, std::string account, std::string password);
	void RemovePlayer(SOCKET socket);

	LoginObj* GetPlayer(SOCKET socket);
	LoginObj* GetPlayer(std::string account);
    size_t Count();

	void BackToPool() override;

private:
	// 正在验证账号
	// <socket, loginPlayer*>
	std::map<SOCKET, LoginObj*> _players;

	// <account, socket>
	std::map <std::string, SOCKET> _accounts;
};

