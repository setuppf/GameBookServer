#include "login_obj.h"

LoginObj::LoginObj(SOCKET socket, std::string account, std::string password)
{
	_socket = socket;
	_account = account;
	_password = password;
}

std::string LoginObj::GetAccount() const
{
	return _account;
}

SOCKET LoginObj::GetSocket()
{
	return _socket;
}
