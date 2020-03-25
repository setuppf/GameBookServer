#pragma once
#include <memory>

#include "disposable.h"
#include "network.h"
#include "time.h"

class RecvNetworkBuffer;
class SendNetworkBuffer;
class Packet;

#define PingTime 1000 // 1√Î
#define PingDelayTime  10 * 1000 // 10√Î

class ConnectObj : public IDisposable
{
public:
    ConnectObj(Network* pNetWork, SOCKET socket);
    ~ConnectObj() override;

    void Dispose() override;

    SOCKET GetSocket() const { return _socket; }
    bool HasRecvData() const;
    Packet* GetRecvPacket() const;
    bool Recv() const;

    bool HasSendData() const;
    void SendPacket(Packet* pPacket) const;
    bool Send() const;
    void Close();

protected:
    Network* _pNetWork{ nullptr };
    const SOCKET _socket;
    RecvNetworkBuffer* _recvBuffer{ nullptr };
    SendNetworkBuffer* _sendBuffer{ nullptr };
};

