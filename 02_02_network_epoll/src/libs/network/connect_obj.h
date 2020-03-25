#pragma once
#include "disposable.h"

class Network;
class RecvNetworkBuffer;
class SendNetworkBuffer;
class Packet;

class ConnectObj : public IDisposable {
public:
	ConnectObj( Network* pNetWork, int socket );
	~ConnectObj( ) override;

	void Dispose( ) override;

	int GetSocket( ) const { return _socket; }
	bool HasRecvData( ) const;
	Packet* GetRecvPacket( ) const;
	bool Recv( ) const;

	bool HasSendData() const;
	void SendPacket( Packet* pPacket ) const;
	bool Send( ) const;

protected:
	Network* _pNetWork{ nullptr };
	const int _socket;
	RecvNetworkBuffer* _recvBuffer{ nullptr };
	SendNetworkBuffer* _sendBuffer{ nullptr };
};

