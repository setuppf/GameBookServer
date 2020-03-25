#include "lobby.h"

#include "libserver/message_system_help.h"

#include "libplayer/player_collector_component.h"
#include "libplayer/player_component_proto_list.h"
#include "libserver/message_system.h"

#include "world_component_gather.h"
#include "player_component_onlinegame.h"
#include "player_component_token.h"
#include "libplayer/player.h"

void Lobby::Awake()
{
    AddComponent<PlayerCollectorComponent>();
    AddComponent<WorldComponentGather>();

    // message
    auto pMsgSystem = GetSystemManager()->GetMessageSystem();    

    pMsgSystem->RegisterFunction(this, Proto::MsgId::MI_NetworkDisconnect, BindFunP1(this, &Lobby::HandleNetworkDisconnect));
    pMsgSystem->RegisterFunction(this, Proto::MsgId::C2G_LoginByToken, BindFunP1(this, &Lobby::HandleLoginByToken));
    pMsgSystem->RegisterFunction(this, Proto::MsgId::MI_GameTokenToRedisRs, BindFunP1(this, &Lobby::HandleGameTokenToRedisRs));
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
    //Proto::QueryPlayer protoQuery;
    //protoQuery.set_player_sn(protoRs.token_info().player_sn());
    //MessageSystemHelp::SendPacket(Proto::MsgId::G2DB_QueryPlayer, protoQuery, APP_DB_MGR);
}
