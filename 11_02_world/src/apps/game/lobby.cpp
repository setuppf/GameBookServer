#include "lobby.h"

#include "libserver/message_system_help.h"

#include "libplayer/player_collector_component.h"
#include "libplayer/player_component_proto_list.h"
#include "libserver/message_system.h"

#include "world_component_gather.h"
#include "player_component_onlinegame.h"
#include "player_component_token.h"
#include "libplayer/player.h"
#include "libplayer/player_component_last_map.h"
#include "world_proxy_help.h"
#include "world_proxy_locator.h"

void Lobby::Awake()
{
    AddComponent<PlayerCollectorComponent>();
    AddComponent<WorldComponentGather>();

    // message
    auto pMsgSystem = GetSystemManager()->GetMessageSystem();    

    pMsgSystem->RegisterFunction(this, Proto::MsgId::MI_NetworkDisconnect, BindFunP1(this, &Lobby::HandleNetworkDisconnect));
    pMsgSystem->RegisterFunction(this, Proto::MsgId::C2G_LoginByToken, BindFunP1(this, &Lobby::HandleLoginByToken));
    pMsgSystem->RegisterFunction(this, Proto::MsgId::MI_GameTokenToRedisRs, BindFunP1(this, &Lobby::HandleGameTokenToRedisRs));
    pMsgSystem->RegisterFunction(this, Proto::MsgId::G2DB_QueryPlayerRs, BindFunP1(this, &Lobby::HandleQueryPlayerRs));
}

void Lobby::BackToPool()
{
    _waitingForWorld.clear();
}

void Lobby::HandleNetworkDisconnect(Packet* pPacket)
{
    GetComponent<PlayerCollectorComponent>()->RemovePlayerBySocket(pPacket->GetSocketKey().Socket);
}

void Lobby::HandleLoginByToken(Packet* pPacket)
{
    auto pPlayerCollector = GetComponent<PlayerCollectorComponent>();

    auto proto = pPacket->ParseToProto<Proto::LoginByToken>();
    auto pPlayer = pPlayerCollector->AddPlayer(pPacket, proto.account());
    if (pPlayer == nullptr)
    {
        MessageSystemHelp::DispatchPacket(Proto::MsgId::MI_NetworkRequestDisconnect, pPacket);
        return;
    }

    pPlayer->AddComponent<PlayerComponentToken>(proto.token());
    pPlayer->AddComponent<PlayerComponentOnlineInGame>(pPlayer->GetAccount(), 1);

    Proto::GameTokenToRedis protoToken;
    protoToken.set_account(pPlayer->GetAccount().c_str());
    MessageSystemHelp::DispatchPacket(Proto::MsgId::MI_GameTokenToRedis, protoToken, nullptr);

}

void Lobby::HandleGameTokenToRedisRs(Packet* pPacket)
{
    auto protoRs = pPacket->ParseToProto<Proto::GameTokenToRedisRs>();
    auto pPlayer = GetComponent<PlayerCollectorComponent>()->GetPlayerByAccount(protoRs.account());
    if (pPlayer == nullptr)
    {
        LOG_ERROR("HandleGameRequestTokenToRedisRs. pPlayer == nullptr. account:" << protoRs.account().c_str());
        return;
    }

    Proto::LoginByTokenRs protoLoginGameRs;
    protoLoginGameRs.set_return_code(Proto::LoginByTokenRs::LGRC_TOKEN_WRONG);
    const auto pTokenComponent = pPlayer->GetComponent<PlayerComponentToken>();
    if (pTokenComponent->IsTokenValid(protoRs.token_info().token()))
    {
        protoLoginGameRs.set_return_code(Proto::LoginByTokenRs::LGRC_OK);
    }

    MessageSystemHelp::SendPacket(Proto::MsgId::C2G_LoginByTokenRs, pPacket, protoLoginGameRs);

    if (protoLoginGameRs.return_code() != Proto::LoginByTokenRs::LGRC_OK)
        return;

    LOG_DEBUG("enter game. account:" << pPlayer->GetAccount().c_str() << " token:" << protoRs.token_info().token().c_str());

    // 查询玩家数据	
    Proto::QueryPlayer protoQuery;
    protoQuery.set_player_sn(protoRs.token_info().player_sn());
    MessageSystemHelp::SendPacket(Proto::MsgId::G2DB_QueryPlayer, protoQuery, APP_DB_MGR);
}

void Lobby::HandleQueryPlayerRs(Packet* pPacket)
{
    auto protoRs = pPacket->ParseToProto<Proto::QueryPlayerRs>();
    auto account = protoRs.account();
    auto pPlayer = GetComponent<PlayerCollectorComponent>()->GetPlayerByAccount(account);
    if (pPlayer == nullptr)
    {
        LOG_ERROR("HandleQueryPlayer. pPlayer == nullptr. account:" << account.c_str());
        return;
    }

    // 向客户端发送玩家数据
    Proto::SyncPlayer syncPlayer;
    syncPlayer.mutable_player()->CopyFrom(protoRs.player());
    MessageSystemHelp::SendPacket(Proto::MsgId::G2C_SyncPlayer, pPlayer, syncPlayer);

    // 分析进入地图
    auto protoPlayer = protoRs.player();
    const auto playerSn = protoPlayer.sn();
    pPlayer->ParserFromProto(playerSn, protoPlayer);
    const auto pPlayerLastMap = pPlayer->AddComponent<PlayerComponentLastMap>();
    auto pWorldLocator = ComponentHelp::GetGlobalEntitySystem()->GetComponent<WorldProxyLocator>();

    // 进入副本
    auto pLastMap = pPlayerLastMap->GetLastDungeon();
    if (pLastMap != nullptr && pWorldLocator->IsExistDungeon(pLastMap->WorldSn))
    {
        // 存在副本，跳转
        WorldProxyHelp::Teleport(pPlayer, GetSN(), pLastMap->WorldSn);
        return;
    }

    // 进入公共地图
    pLastMap = pPlayerLastMap->GetLastPublicMap();
    const auto lastMapSn = pWorldLocator->GetWorldSnById(pLastMap->WorldId);
    if (lastMapSn != (uint64)INVALID_ID)
    {
        // 存在公共地图，跳转
        WorldProxyHelp::Teleport(pPlayer, GetSN(), lastMapSn);
        return;
    }

    // 等待跳转
    if (_waitingForWorld.find(pLastMap->WorldId) == _waitingForWorld.end())
    {
        _waitingForWorld[pLastMap->WorldId] = std::set<uint64>();
    }

    _waitingForWorld[pLastMap->WorldId].insert(pPlayer->GetPlayerSN());

    // 向appmgr申请创建地图
    Proto::RequestWorld protoToMgr;
    protoToMgr.set_world_id(pLastMap->WorldId);
    MessageSystemHelp::SendPacket(Proto::MsgId::G2M_RequestWorld, protoToMgr, APP_APPMGR);
}
