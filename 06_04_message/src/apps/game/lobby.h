#pragma once
#include "libplayer/world_base.h"
#include "libserver/system.h"
#include "libserver/entity.h"

class Packet;
class Player;

class Lobby : public Entity<Lobby>, public IWorld, public IAwakeSystem<>
{
public:
	void Awake() override;
	void BackToPool() override;

private:
    void HandleNetworkDisconnect(Packet* pPacket);
    void HandleLoginByToken(Packet* pPacket);

private:
	std::map<int, std::set<uint64>> _waitingForWorld;
};
