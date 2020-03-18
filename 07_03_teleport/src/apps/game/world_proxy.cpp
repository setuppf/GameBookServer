#include "world_proxy.h"
#include "world_proxy_locator.h"
#include "player_component_onlinegame.h"

#include "libserver/component_help.h"
#include "libserver/message_system_help.h"
#include "libserver/message_system.h"

#include "libplayer/player_collector_component.h"
#include "libplayer/player.h"
#include "libserver/log4.h"
#include "world_component_gather.h"

void WorldProxy::Awake(int worldId, uint64 lastWorldSn)
{
    _spaceAppId = Global::GetAppIdFromSN(_sn);
    LOG_DEBUG("create world proxy. world id:" << worldId << " sn:" << _sn << " space app id:" << _spaceAppId);

    _worldId = worldId;

    AddComponent<PlayerCollectorComponent>();
    AddComponent<WorldComponentGather>();

    auto pProxyLocator = ComponentHelp::GetGlobalEntitySystem()->GetComponent<WorldProxyLocator>();
    pProxyLocator->RegisterToLocator(_worldId, GetSN());

    // 广播给所有进程
    Proto::BroadcastCreateWorldProxy protoCreate;
    protoCreate.set_world_id(_worldId);
    protoCreate.set_world_sn(GetSN());
    MessageSystemHelp::DispatchPacket(Proto::MsgId::MI_BroadcastCreateWorldProxy, protoCreate, nullptr);

    // message
    auto pMsgSystem = GetSystemManager()->GetMessageSystem();
    pMsgSystem->RegisterFunction(this, Proto::MsgId::MI_NetworkDisconnect, BindFunP1(this, &WorldProxy::HandleNetworkDisconnect));
    pMsgSystem->RegisterFunction(this, Proto::MsgId::MI_Teleport, BindFunP1(this, &WorldProxy::HandleTeleport));

    // 默认协议处理函数
    pMsgSystem->RegisterDefaultFunction(this, BindFunP1(this, &WorldProxy::HandleDefaultFunction));
}

void WorldProxy::BackToPool()
{

}

void WorldProxy::SendPacketToWorld(const Proto::MsgId msgId, ::google::protobuf::Message& proto, Player* pPlayer) const
{
    TagKey tagKey;
    tagKey.AddTag(TagType::Player, pPlayer->GetPlayerSN());
    tagKey.AddTag(TagType::Entity, _sn);
    if (Global::GetInstance()->GetCurAppType() == APP_ALLINONE)
    {
        tagKey.AddTag(TagType::ToWorld, _sn);
    }
    MessageSystemHelp::SendPacket(msgId, proto, &tagKey, APP_SPACE, _spaceAppId);
}

void WorldProxy::CopyPacketToWorld(Player* pPlayer, Packet* pPacket) const
{
    auto pPacketCopy = MessageSystemHelp::CreatePacket((Proto::MsgId)pPacket->GetMsgId(), nullptr);
    pPacketCopy->CopyFrom(pPacket);
    auto pTagKey = pPacketCopy->GetTagKey();
    pTagKey->AddTag(TagType::Player, pPlayer->GetPlayerSN());
    pTagKey->AddTag(TagType::Entity, _sn);
    if (Global::GetInstance()->GetCurAppType() == APP_ALLINONE)
    {
        pTagKey->AddTag(TagType::ToWorld, _sn);
    }

    MessageSystemHelp::SendPacket(pPacketCopy, APP_SPACE, _spaceAppId);
}

void WorldProxy::HandleDefaultFunction(Packet* pPacket)
{
    auto pPlayerMgr = this->GetComponent<PlayerCollectorComponent>();
    Player* pPlayer = nullptr;
    const auto pTagKey = pPacket->GetTagKey();
    if (pTagKey == nullptr)
    {
        LOG_ERROR("world proxy recv msg. but no tag. msgId:" << Log4Help::GetMsgIdName(pPacket->GetMsgId()).c_str());
        return;
    }

    bool isToClient = false;
    const auto pTagPlayer = pTagKey->GetTagValue(TagType::Player);
    if (pTagPlayer != nullptr)
    {
        isToClient = true;
        pPlayer = pPlayerMgr->GetPlayerBySn(pTagPlayer->KeyInt64);
    }
    else
    {
        pPlayer = pPlayerMgr->GetPlayerBySocket(pPacket->GetSocketKey()->Socket);
    }

    // 有可能协议传来时，已经断线了
    if (pPlayer == nullptr)
        return;

    // 默认只作中转操作
    if (isToClient)
    {
        auto pPacketCopy = MessageSystemHelp::CreatePacket((Proto::MsgId)pPacket->GetMsgId(), pPlayer);
        pPacketCopy->CopyFrom(pPacket);
        MessageSystemHelp::SendPacket(pPacketCopy);

        LOG_DEBUG("transfer msg to client. msgId:" << Log4Help::GetMsgIdName(pPacket->GetMsgId()).c_str());
    }
    else
    {
        CopyPacketToWorld(pPlayer, pPacket);
        LOG_DEBUG("transfer msg to space. msgId:" << Log4Help::GetMsgIdName(pPacket->GetMsgId()).c_str())
    }
}

void WorldProxy::HandleNetworkDisconnect(Packet* pPacket)
{
    TagValue* pTagValue = pPacket->GetTagKey()->GetTagValue(TagType::Account);
    if (pTagValue != nullptr)
    {
        auto pCollector = GetComponent<PlayerCollectorComponent>();
        pCollector->RemovePlayerBySocket(pPacket->GetSocketKey()->Socket);
    }
    else
    {
        // 可能是space, login, appmgr，dbmgr断线
        // ……
    }
}

void WorldProxy::HandleTeleport(Packet* pPacket)
{
    auto proto = pPacket->ParseToProto<Proto::Teleport>();
    const auto playerSn = proto.player_sn();

    auto pCollector = GetComponent<PlayerCollectorComponent>();
    auto pPlayer = pCollector->AddPlayer(pPacket, proto.account());
    if (pPlayer == nullptr)
    {
        LOG_ERROR("failed to teleport, account:" << proto.account().c_str());
        return;
    }

    pPlayer->ParserFromProto(playerSn, proto.player());
    pPlayer->AddComponent<PlayerComponentOnlineInGame>(pPlayer->GetAccount());

    LOG_DEBUG("world proxy. recv teleport. map id:" << _worldId << " world sn:" << GetSN() << " account:" << pPlayer->GetAccount().c_str());

    // 将数据转给真实的world
    Proto::SyncPlayer protoSync;
    protoSync.set_account(proto.account().c_str());
    protoSync.mutable_player()->CopyFrom(proto.player());
    SendPacketToWorld(Proto::MsgId::G2S_SyncPlayer, protoSync, pPlayer);

    // 通知旧地图，跳转成功
    Proto::TeleportAfter protoTeleportRs;
    protoTeleportRs.set_player_sn(pPlayer->GetPlayerSN());
    NetIdentify indentify;
    indentify.GetTagKey()->AddTag(TagType::Player, pPlayer->GetPlayerSN());
    indentify.GetTagKey()->AddTag(TagType::Entity, proto.last_world_sn());
    MessageSystemHelp::DispatchPacket(Proto::MsgId::MI_TeleportAfter, protoTeleportRs, &indentify);
}
