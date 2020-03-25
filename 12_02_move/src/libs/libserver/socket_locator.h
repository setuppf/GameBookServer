#pragma once
#include "system.h"
#include "entity.h"

class Packet;

class SocketLocator :public Entity<SocketLocator>, public IAwakeSystem<>
{
public:
    void Awake() override;
    void BackToPool() override;

    void RegisterToLocator(SOCKET socket, uint64 sn);
    void Remove(SOCKET socket);
    uint64 GetTargetEntitySn(SOCKET socket);

private:
    void HandleNetworkDisconnect(Packet* pPacket);

private:
    std::mutex _lock;
    std::map<SOCKET, uint64> _componentes;
};

