#include "player.h"

Player::Player(SOCKET socket, std::string account, std::string password)
{
	_socket = socket;
	_account = account;
	_password = password;
}

std::string Player::GetAccount() const
{
	return _account;
}

SOCKET Player::GetSocket()
{
	return _socket;
}
