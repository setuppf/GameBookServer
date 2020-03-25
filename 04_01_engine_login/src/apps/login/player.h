#pragma once
#include "libserver/socket_object.h"
#include "libserver/common.h"
#include <string>

class Player :public ISocketObject
{
public:
	Player(SOCKET socketInfo, std::string account, std::string password);
	std::string GetAccount() const;
	SOCKET GetSocket() override;

private:
	std::string _password;
	std::string _account;

	SOCKET _socket;
};

