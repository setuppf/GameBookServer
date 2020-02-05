#include "client_socket.h"
#include "network/network.h"
#include <string>
#include <random>

ClientSocket::ClientSocket(int index) : _curIndex(index)
{
	_thread = std::thread([index, this]()
	{
		_isRun = true;
		this->MsgHandler();
		_isRun = false;
	});
}

bool ClientSocket::IsRun() const
{
	return _isRun;
}

void ClientSocket::Stop()
{
	if (_thread.joinable())
		_thread.join();
}


void ClientSocket::MsgHandler()
{
	_sock_init();
	SOCKET socket = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (socket == INVALID_SOCKET)
	{
		std::cout << "::socket failed. err:" << _sock_err() << std::endl;
		return;
	}

	sockaddr_in addr;
	memset(&addr, 0, sizeof(sockaddr_in));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(2233);
	::inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr.s_addr);

	if (::connect(socket, (struct sockaddr *)&addr, sizeof(sockaddr)) < 0)
	{
		std::cout << "::connect failed. err:" << _sock_err() << std::endl;
		return;
	}

	char buffer[1024];
	memset(buffer, 0, sizeof(buffer));

	std::string msg = "ping_" + std::to_string(_curIndex);
	std::cout << "::send." << msg.c_str() << " socket:" << socket << std::endl;
	::send(socket, msg.c_str(), msg.length(), 0);

	memset(&buffer, 0, sizeof(buffer));
	::recv(socket, buffer, sizeof(buffer), 0);
	std::cout << "::recv." << buffer << " socket:" << socket << std::endl << std::endl;

	_sock_close(socket);
	_sock_exit();
}

