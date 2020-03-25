#pragma once
#include <memory>

#include "disposable.h"
#include "network.h"
#include "time.h"
#include "object_block.h"

class RecvNetworkBuffer;
class SendNetworkBuffer;
class Packet;

#define PingTime 1000 // 1Ãë
#define PingDelayTime  10 * 1000 // 10Ãë

class ConnectObj : public ObjectBlock
{
public:
    ConnectObj(IDynamicObjectPool* pPool);
    virtual ~ConnectObj();

    SOCKET GetSocket() const { return _socket; }
    bool HasRecvData() const;
    Packet* GetRecvPacket() const;
    bool Recv() const;

    bool HasSendData() const;
    void SendPacket(Packet* pPacket) const;
    bool Send() const;
    void Close();

    // Í¨¹ý ObjectBlock ¼Ì³Ð
    void TakeoutFromPool(Network* pNetWork, SOCKET socket);
    virtual void BackToPool() override;

protected:
    Network* _pNetWork{ nullptr };
    SOCKET _socket;
    RecvNetworkBuffer* _recvBuffer{ nullptr };
    SendNetworkBuffer* _sendBuffer{ nullptr };
};

