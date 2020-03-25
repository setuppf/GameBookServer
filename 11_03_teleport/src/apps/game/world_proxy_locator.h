#pragma once
#include "libserver/entity.h"

class Packet;

class WorldProxyLocator :public Entity<WorldProxyLocator>, public IAwakeFromPoolSystem<>
{
public:
	void Awake() override;
    void BackToPool() override;

	void RegisterToLocator(int worldId, uint64 worldSn);
	void Remove(int worldId, uint64 worldSn);

	bool IsExistDungeon(uint64 worldSn);
	uint64 GetWorldSnById(int worldId);

private:
	void HandleBroadcastCreateWorld(Packet* pPacket);

private:
	std::mutex _lock;

	// <world id, world sn>
	std::map<int, uint64> _publics; // 公共地图

	// <world sn>
	std::set<uint64> _worlds;  // 副本只存sn
};

