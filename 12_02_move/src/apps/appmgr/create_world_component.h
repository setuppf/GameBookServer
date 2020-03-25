#pragma once
#include "libserver/entity.h"
#include "libserver/sync_component.h"

class Packet;

class CreateWorldComponent :public SyncComponent, public Entity<CreateWorldComponent>, public IAwakeSystem<>
{
public:
	void Awake() override;
    void BackToPool() override;

private:
    int ReqCreateWorld(int worldId);

    void HandleCmdCreate(Packet* pPacket);
    void HandleAppInfoSync(Packet* pPacket);
	void HandleNetworkDisconnect(Packet* pPacket) override;
	void HandleRequestWorld(Packet* pPacket);
	void HandleQueryWorld(Packet* pPacket);
	void HandleBroadcastCreateWorld(Packet* pPacket);

private:
	// <world id, space Id>
	std::map<int, int> _creating;

	// <world id, world sn>
	std::map<int, uint64> _created;  // 创建的公共地图信息

	// <world sn, world id>
	std::map<uint64, int> _dungeons;	// 创建的副本地图
};

