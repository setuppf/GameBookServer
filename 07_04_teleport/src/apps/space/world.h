#pragma once
#include "libserver/system.h"
#include "libplayer/world_base.h"
#include "libserver/entity.h"
#include "libserver/socket_object.h"

class Player;
class World :public Entity<World>, public IWorld, public IAwakeFromPoolSystem<int>
{
public:
    void Awake(int worldId) override;
    void BackToPool() override;

protected:
    Player* GetPlayer(NetIdentify* pIdentify);

    void BroadcastPacket(Proto::MsgId msgId, google::protobuf::Message& proto);
    void BroadcastPacket(Proto::MsgId msgId, google::protobuf::Message& proto, std::set<uint64> players);

    void HandleNetworkDisconnect(Packet* pPacket);
    void HandleSyncPlayer(Packet* pPacket);
    void HandleRequestSyncPlayer(Player* pPlayer, Packet* pPacket);
    void HandleG2SRemovePlayer(Player* pPlayer, Packet* pPacket);

private:
    void SyncWorldToGather();
    void SyncAppearTimer();

private:
    // 缓存1秒内增加或是删除的玩家
    std::set<uint64> _addPlayer;
};

