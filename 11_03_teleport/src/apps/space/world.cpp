#include "world.h"
#include "player_manager_component.h"
#include "player_component_detail.h"

#include "libserver/message_system_help.h"
#include "libserver/message_system.h"
#include "libplayer/player_component_last_map.h"

void World::Awake(int worldId)
{
    LOG_DEBUG("create world. id:" << worldId << " sn:" << _sn << " space app id:" << Global::GetAppIdFromSN(_sn));

    _worldId = worldId;

    AddComponent<PlayerManagerComponent>();

    AddTimer(0, 10, false, 0, BindFunP0(this, &World::SyncWorldToGather));
    AddTimer(0, 1, false, 0, BindFunP0(this, &World::SyncAppearTimer));

    // message
    auto pMsgSystem = GetSystemManager()->GetMessageSystem();
    pMsgSystem->RegisterFunction(this, Proto::MsgId::G2S_SyncPlayer, BindFunP1(this, &World::HandleSyncPlayer));
}

void World::BackToPool()
{
    _addPlayer.clear();
    _remvePlaery.clear();
}

void World::SyncWorldToGather()
{
    Proto::WorldSyncToGather proto;
    proto.set_world_sn(GetSN());
    proto.set_world_id(GetWorldId());

    const int online = GetComponent<PlayerManagerComponent>()->OnlineSize();
    proto.set_online(online);

    MessageSystemHelp::DispatchPacket(Proto::MsgId::MI_WorldSyncToGather, proto, nullptr);
}

inline void CreateProtoRoleAppear(Player* pPlayer, Proto::RoleAppear& protoAppear)
{
    auto proto = protoAppear.add_role();
    proto->set_name(pPlayer->GetName().c_str());
    proto->set_sn(pPlayer->GetPlayerSN());

    const auto pBaseInfo = pPlayer->GetComponent<PlayerComponentDetail>();
    proto->set_gender(pBaseInfo->GetGender());

    const auto pComponentLastMap = pPlayer->GetComponent<PlayerComponentLastMap>();
    const auto pLastMap = pComponentLastMap->GetCur();
    pLastMap->Position.SerializeToProto(proto->mutable_position());
}

void World::SyncAppearTimer()
{
    auto pPlayerMgr = GetComponent<PlayerManagerComponent>();

    if (_addPlayer.size() > 0)
    {
        // 1.新增的数据，同步到全地图
        Proto::RoleAppear protoNewAppear;
        for (auto id : _addPlayer)
        {
            // 有可能瞬间已下线
            auto pPlayer = pPlayerMgr->GetPlayerBySn(id);
            if (pPlayer == nullptr)
                continue;

            CreateProtoRoleAppear(pPlayer, protoNewAppear);
        }

        BroadcastPacket(Proto::MsgId::S2C_RoleAppear, protoNewAppear);

        // 2.原始玩家的数据，同步给新增的玩家
        Proto::RoleAppear protoOther;
        const auto players = pPlayerMgr->GetAll();
        for (const auto one : *players)
        {
            // 排除新玩家
            if (_addPlayer.find(one.first) != _addPlayer.end())
                continue;

            const auto role = one.second;
            CreateProtoRoleAppear(role, protoOther);
        }

        BroadcastPacket(Proto::MsgId::S2C_RoleAppear, protoOther, _addPlayer);

        _addPlayer.clear();
    }
}

void World::HandleSyncPlayer(Packet* pPacket)
{
    auto proto = pPacket->ParseToProto<Proto::SyncPlayer>();
    const auto playerSn = proto.player().sn();
    //const int gameAppId = proto.app_id();

    auto pPlayerMgr = GetComponent<PlayerManagerComponent>();
    auto pPlayer = pPlayerMgr->AddPlayer(playerSn, GetSN(), pPacket);
    if (pPlayer == nullptr)
    {
        LOG_ERROR("failed to add player. player sn:" << playerSn);
        return;
    }

    pPlayer->ParserFromProto(playerSn, proto.player());
    pPlayer->AddComponent<PlayerComponentDetail>();

    const auto pComponentLastMap = pPlayer->AddComponent<PlayerComponentLastMap>();
    pComponentLastMap->EnterWorld(_worldId, _sn);
    const auto pLastMap = pComponentLastMap->GetCur();

    LOG_DEBUG("world. recv teleport. map id:" << _worldId << " world sn:" << GetSN() << " playerSn:" << pPlayer->GetPlayerSN());

    //通知客户端进入地图
    Proto::EnterWorld protoEnterWorld;
    protoEnterWorld.set_world_id(_worldId);
    pLastMap->Position.SerializeToProto(protoEnterWorld.mutable_position());
    MessageSystemHelp::SendPacket(Proto::MsgId::S2C_EnterWorld, protoEnterWorld, pPlayer);

    _addPlayer.insert(playerSn);
}

void World::BroadcastPacket(Proto::MsgId msgId, google::protobuf::Message& proto)
{
    auto pPlayerMgr = GetComponent<PlayerManagerComponent>();
    const auto players = pPlayerMgr->GetAll();
    for (const auto pair : *players)
    {
        //LOG_DEBUG("broadcast msgId:" << Log4Help::GetMsgIdName(msgId).c_str() << " player sn:" << pair.second->GetPlayerSN());
        MessageSystemHelp::SendPacket(msgId, proto, pair.second);
    }
}

void World::BroadcastPacket(Proto::MsgId msgId, google::protobuf::Message& proto, std::set<uint64> players)
{
    auto pPlayerMgr = GetComponent<PlayerManagerComponent>();
    for (const auto one : players)
    {
        const auto pPlayer = pPlayerMgr->GetPlayerBySn(one);
        if (pPlayer == nullptr)
            continue;

        //LOG_DEBUG("broadcast msgId:" << Log4Help::GetMsgIdName(msgId).c_str() << " player sn:" << one);
        MessageSystemHelp::SendPacket(msgId, proto, pPlayer);
    }
}