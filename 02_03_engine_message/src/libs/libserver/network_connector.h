#pragma once
#include "network.h"

class ConnectObj;
class Packet;

class NetworkConnector : public Network
{
public:
	bool Init( ) override;
	void RegisterMsgFunction( ) override;
	
	virtual bool Connect(std::string ip, int port);
	void Update( ) override;

	bool HasRecvData();
	Packet* GetRecvPacket();
	void SendPacket(Packet* pPacket);

	bool IsConnected() const;

private:	
	ConnectObj* GetConnectObj();
	void TryCreateConnectObj();

protected:
	std::string _ip{ "" };
	int _port{ 0 };
};

