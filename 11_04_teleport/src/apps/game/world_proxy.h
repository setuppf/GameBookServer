#pragma once
#include "libserver/entity.h"
#include "libserver/socket_object.h"

#include "libplayer/world_base.h"

class Player;

class WorldProxy :public IWorld, public Entity<WorldProxy>, public IAwakeFromPoolSystem<int, uint64>
{
public:
    void Awake(int worldId, uint64 lastWorldSn) override;
    void BackToPool() override;

    void SendPacketToWorld(const Proto::MsgId msgId, ::google::protobuf::Message& proto, Player* pPlayer) const;
    void SendPacketToWorld(const Proto::MsgId msgId, Player* pPlayer) const;

    void CopyPacketToWorld(Player* pPlayer, Packet* pPacket) const;

private:
    Player* GetPlayer(NetIdentify* pIdentify);

    void HandleNetworkDisconnect(Packet* pPacket);
    void HandleTeleport(Packet* pPacket);
    void HandleTeleportAfter(Player* pPlayer, Packet* pPacket);
	void HandleBroadcastCreateWorldProxy(Packet* pPacket);
    void HandleC2GEnterWorld(Player* pPlayer, Packet* pPacket);
    void HandleS2GSyncPlayer(Player* pPlayer, Packet* pPacket);

    // 协议默认处理函数
    void HandleDefaultFunction(Packet* pPacket);

private:
    int _spaceAppId{ 0 };
};

