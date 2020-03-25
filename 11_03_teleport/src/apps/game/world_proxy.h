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
    void CopyPacketToWorld(Player* pPlayer, Packet* pPacket) const;

private:
    void HandleNetworkDisconnect(Packet* pPacket);
    void HandleTeleport(Packet* pPacket);

    // 协议默认处理函数
    void HandleDefaultFunction(Packet* pPacket);

private:
    int _spaceAppId{ 0 };
};

