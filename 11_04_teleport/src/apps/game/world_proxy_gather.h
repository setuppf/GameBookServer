#pragma once

#include "libserver/system.h"
#include "libserver/entity.h"

class Packet;

struct WorldProxyInfo
{
	uint64 WorldSn;
	int WorldId;
	int Online;
};

class WorldProxyGather :public Entity<WorldProxyGather>, public IAwakeSystem<>
{
public:
	void Awake() override;
    void BackToPool() override;

private:
	void SyncGameInfo();
	void HandleWorldProxySyncToGather(Packet* pPacket);
	void HandleCmdWorldProxy(Packet* pPacket);

private:
	std::map<uint64, WorldProxyInfo> _maps;
};
