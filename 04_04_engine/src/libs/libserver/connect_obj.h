#pragma once
#include <memory>

#include "network.h"
#include "time.h"
#include "entity.h"
#include "system.h"

class RecvNetworkBuffer;
class SendNetworkBuffer;
class Packet;

#define PingTime 1000 // 1√Î
#define PingDelayTime  10 * 1000 // 10√Î

class ConnectObj : public Entity<ConnectObj>, public IAwakeFromPoolSystem<SOCKET>
{
public:
	ConnectObj();
	virtual ~ConnectObj();

	void AwakeFromPool(SOCKET socket);
	virtual void BackToPool() override;

	SOCKET GetSocket() const { return _socket; }
	bool HasRecvData() const;
	Packet* GetRecvPacket() const;
	bool Recv();

	bool HasSendData() const;
	void SendPacket(Packet* pPacket) const;
	bool Send() const;
	void Close();

protected:
	SOCKET _socket;
	RecvNetworkBuffer* _recvBuffer{ nullptr };
	SendNetworkBuffer* _sendBuffer{ nullptr };
};

