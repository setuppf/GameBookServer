
#include <iostream>

#include "connect_obj.h"
#include "network_connector.h"
#include "packet.h"


bool NetworkConnector::Init( ) {
	return true;
}

void NetworkConnector::RegisterMsgFunction( ) { 

}

bool NetworkConnector::IsConnected() const
{
	return _connects.size() > 0;
}

ConnectObj* NetworkConnector::GetConnectObj()
{
	auto iter = _connects.find(_masterSocket);
	if (iter == _connects.end())
	{
		std::cout << "Error. NetworkConnectorpConnect == nullptr!!!!" << std::endl;
		return nullptr;
	}

	return iter->second;
}

bool NetworkConnector::Connect(std::string ip, int port)
{
	_ip = ip;
	_port = port;

	if (_port == 0 || _ip == "")
		return false;

	_masterSocket = CreateSocket();
	if (_masterSocket == INVALID_SOCKET)
		return false;

#ifdef EPOLL
	//std::cout << "epoll model" << std::endl;
	InitEpoll();
#else
	//std::cout << "select model" << std::endl;
#endif

	sockaddr_in addr;
	memset(&addr, 0, sizeof(sockaddr_in));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(_port);
	::inet_pton(AF_INET, _ip.c_str(), &addr.sin_addr.s_addr);

	int rs = ::connect(_masterSocket, (struct sockaddr *)&addr, sizeof(sockaddr));
	if (rs == 0)
	{
		// 成功
		CreateConnectObj(_masterSocket);
	}

	return true;
}

void NetworkConnector::TryCreateConnectObj()
{
	int optval = -1;
	socklen_t optlen = sizeof(optval);
	int rs = ::getsockopt(_masterSocket, SOL_SOCKET, SO_ERROR, (char*)(&optval), &optlen);
	if (rs == 0 && optval == 0)
	{
		CreateConnectObj(_masterSocket);
	}
	else
	{
		std::cout << "connect failed. socket:" << _masterSocket << std::endl;
		Dispose();
	}
}

#ifdef EPOLL

void NetworkConnector::Update()
{
	// 如果断线，重新连接
	if (_masterSocket == INVALID_SOCKET)
	{
		if (!Connect(_ip, _port))
			return;

		std::cout << "re connect. socket:" << _masterSocket << std::endl;
	}

	Epoll();

	if (IsConnected())
		return;

	if (_mainSocketEventIndex >= 0)
	{
		int fd = _events[_mainSocketEventIndex].data.fd;
		if (fd != _masterSocket)
			return;

		// connect成功，会触发IN事件
		if (_events[_mainSocketEventIndex].events & EPOLLIN || _events[_mainSocketEventIndex].events & EPOLLOUT)
		{
			TryCreateConnectObj();
		}
	}
}

#else

void NetworkConnector::Update()
{
	// 如果断线，重新连接
	if (_masterSocket == INVALID_SOCKET)
	{
		if (!Connect(_ip, _port))
			return;

		std::cout << "re connect. socket:" << _masterSocket << std::endl;
	}

	Select();

	if (!IsConnected())
	{
		// 有异常出现
		if (FD_ISSET(_masterSocket, &exceptfds))
		{
			std::cout << "connect except. socket:" << _masterSocket << " re connect." << std::endl;

			// 关闭当前socket，重新connect
			Dispose();
			return;
		}

		if (FD_ISSET(_masterSocket, &readfds) || FD_ISSET(_masterSocket, &writefds))
		{
			TryCreateConnectObj();
		}
	}
}

#endif

bool NetworkConnector::HasRecvData()
{
	auto size = _connects.size();
	if (size <= 0)
		return false;

	if (size != 1)
	{
		std::cout << "Error. NetworkConnector has two connect!!!!" << std::endl;
		return false;
	}

	ConnectObj* pConnect = GetConnectObj();
	if (pConnect == nullptr)
		return false;

	return pConnect->HasRecvData();
}

Packet* NetworkConnector::GetRecvPacket()
{
	ConnectObj* pConnect = GetConnectObj();
	if (pConnect == nullptr)
		return nullptr;

	return pConnect->GetRecvPacket();
}

void NetworkConnector::SendPacket(Packet* pPacket)
{
	ConnectObj* pConnect = GetConnectObj();
	if (pConnect == nullptr)
		return;

	pConnect->SendPacket(pPacket);
}
