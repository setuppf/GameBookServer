#pragma once
#include "libserver/system.h"
#include "libplayer/world_base.h"
#include "libserver/entity.h"
#include "libserver/socket_object.h"

class World :public Entity<World>, public IWorld, public IAwakeFromPoolSystem<int>
{
public:
    void Awake(int worldId) override;
    void BackToPool() override;

protected:
    void HandleSyncPlayer(Packet* pPacket);
    void BroadcastPacket(Proto::MsgId msgId, google::protobuf::Message& proto);
    void BroadcastPacket(Proto::MsgId msgId, google::protobuf::Message& proto, std::set<uint64> players);

private:
    void SyncWorldToGather();
    void SyncAppearTimer();

private:
    // 缓存1秒内增加或是删除的玩家
    std::set<uint64> _addPlayer;
    std::set<uint64> _remvePlaery;
};

