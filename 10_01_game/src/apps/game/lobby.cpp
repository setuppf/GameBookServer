#include "lobby.h"

#include "libserver/message_component.h"
#include "libserver/message_system_help.h"

#include "libplayer/player_collector_component.h"
#include "libplayer/player_component_proto_list.h"

#include "world_component_gather.h"

void Lobby::Awake()
{
    AddComponent<PlayerCollectorComponent>();
    AddComponent<WorldComponentGather>();

    // message
    auto pMsgCallBack = new MessageCallBackFunction();
    AddComponent<MessageComponent>(pMsgCallBack);

    pMsgCallBack->RegisterFunction(Proto::MsgId::MI_NetworkDisconnect, BindFunP1(this, &Lobby::HandleNetworkDisconnect));
    pMsgCallBack->RegisterFunction(Proto::MsgId::C2G_LoginByToken, BindFunP1(this, &Lobby::HandleLoginByToken));
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

    LOG_DEBUG("enter game. account:" << proto.account().c_str());

    Proto::LoginByTokenRs protoLoginGameRs;
    protoLoginGameRs.set_return_code(Proto::LoginByTokenRs::LGRC_OK);
    MessageSystemHelp::SendPacket(Proto::MsgId::C2G_LoginByTokenRs, pPacket, protoLoginGameRs);
}
