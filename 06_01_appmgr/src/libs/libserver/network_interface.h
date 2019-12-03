#pragma once
#include "socket_object.h"

class Packet;

enum class NetworkType
{
	None = 1 << 0,
	TcpListen = 1 << 1,
	TcpConnector = 1 << 2,

	HttpListen = 1 << 3,
	HttpConnector = 1 << 4,
};

class INetwork :public ISocketObject
{
public:
	virtual ~INetwork() = default;
	virtual void SendPacket(Packet*& pPacket) = 0;
};
