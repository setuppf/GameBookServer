#pragma once
#include "socket_object.h"

class Packet;

enum NetworkType
{
	NetworkTypeNone = 1 << 0,
	NetworkTcpListen = 1 << 1,
	NetworkTcpConnector = 1 << 2,

	NetworkHttpListen = 1 << 3,
	NetworkHttpConnector = 1 << 4,
};

class INetwork :public ISocketObject
{
public:
	virtual ~INetwork() = default;
	virtual void SendPacket(Packet*& pPacket) = 0;
};
