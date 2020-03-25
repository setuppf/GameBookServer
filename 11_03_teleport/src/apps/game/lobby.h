#pragma once
#include "libplayer/world_base.h"
#include "libserver/system.h"
#include "libserver/entity.h"
#include "libserver/socket_object.h"

class Packet;
class Player;

class Lobby : public Entity<Lobby>, public IWorld, public IAwakeSystem<>
{
public:
	void Awake() override;
	void BackToPool() override;

private:
    Player* GetPlayer(NetIdentify* pIdentify);

    void HandleNetworkDisconnect(Packet* pPacket);
    void HandleLoginByToken(Packet* pPacket);
    void HandleGameTokenToRedisRs(Packet* pPacket);
    void HandleQueryPlayerRs(Packet* pPacket);
    void HandleBroadcastCreateWorldProxy(Packet* pPacket);
    void HandleTeleportAfter(Player* pPlayer, Packet* pPacket);

private:
	std::map<int, std::set<uint64>> _waitingForWorld;
};

