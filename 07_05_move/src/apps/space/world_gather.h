#pragma once

#include "libserver/entity.h"
#include "libserver/system.h"

class Packet;

class WorldGather :public Entity<WorldGather>, public IAwakeFromPoolSystem<>
{
public:
	void Awake() override;
	void BackToPool() override;

private:
	void SyncSpaceInfo();
	void HandleWorldSyncToGather(Packet* pPacket);
	void HandleCmdWorld(Packet* pPacket);

private:
	// <map sn, map player count>
	std::map<uint64, int> _maps;
};
