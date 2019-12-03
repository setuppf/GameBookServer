#pragma once
#include "network.h"

class ConnectObj;
class Packet;

class NetworkConnector : public Network
{
public:
	bool Connect(std::string ip, int port);
	bool Update();

	bool HasRecvData();
	Packet* GetRecvPacket();
	void SendPacket(Packet* pPacket);

	bool IsConnected() const;

private:
	ConnectObj* GetConnectObj();

private:
	std::string _ip;
	int _port;
};

