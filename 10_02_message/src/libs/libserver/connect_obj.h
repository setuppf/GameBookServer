#pragma once

#include "common.h"
#include "entity.h"
#include "system.h"
#include "socket_object.h"

class RecvNetworkBuffer;
class SendNetworkBuffer;
class Packet;

#define PingTime 1000 // 1√Î
#define PingDelayTime  10 * 1000 // 10√Î

enum class ConnectStateType
{
    None,
    Connecting,
    Connected,
};

class ConnectObj : public Entity<ConnectObj>, public NetworkIdentify, public IAwakeFromPoolSystem<SOCKET, NetworkType, ObjectKey, ConnectStateType>
{
public:
    ConnectObj();
    virtual ~ConnectObj();

    void Awake(SOCKET socket, NetworkType networkType, ObjectKey key, ConnectStateType state) override;
    virtual void BackToPool() override;

    bool HasRecvData() const;
    bool Recv();

    bool HasSendData() const;
    void SendPacket(Packet* pPacket) const;
    bool Send();
    void Close();
    ConnectStateType GetState() const;
    void ChangeStateToConnected();
    void ModifyConnectKey(ObjectKey key);

protected:
    ConnectStateType _state{ ConnectStateType::None };

    RecvNetworkBuffer* _recvBuffer{ nullptr };
    SendNetworkBuffer* _sendBuffer{ nullptr };
};

