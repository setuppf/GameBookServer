#include "world_proxy.h"
#include "world_proxy_locator.h"
#include "player_component_onlinegame.h"

#include "libserver/component_help.h"
#include "libserver/message_system_help.h"
#include "libserver/message_system.h"

#include "libplayer/player_collector_component.h"
#include "libplayer/player.h"

#include "libserver/log4.h"
#include "libserver/network_help.h"

#include "world_proxy_component_gather.h"
#include "world_component_teleport.h"
#include "libresource/resource_help.h"

void WorldProxy::Awake(int worldId, uint64 lastWorldSn)
{
    _worldId = worldId;

    _spaceAppId = Global::GetAppIdFromSN(_sn);
    LOG_DEBUG("create world proxy. world id:" << worldId << " sn:" << _sn << " space app id:" << _spaceAppId);

    AddComponent<PlayerCollectorComponent>();
    AddComponent<WorldProxyComponentGather>();
    AddComponent<WorldComponentTeleport>();

    auto pProxyLocator = ComponentHelp::GetGlobalEntitySystem()->GetComponent<WorldProxyLocator>();
    pProxyLocator->RegisterToLocator(_worldId, GetSN());

    // 广播给所有进程
    Proto::BroadcastCreateWorldProxy protoCreate;
    protoCreate.set_world_id(_worldId);
    protoCreate.set_world_sn(GetSN());

    if (lastWorldSn > 0)
    {
        NetIdentify netIdentify;
        netIdentify.GetTagKey()->AddTag(TagType::Entity, lastWorldSn);
        MessageSystemHelp::DispatchPacket(Proto::MsgId::MI_BroadcastCreateWorldProxy, protoCreate, &netIdentify);
    }
    else
    {
        MessageSystemHelp::DispatchPacket(Proto::MsgId::MI_BroadcastCreateWorldProxy, protoCreate, nullptr);
    }

    // message
    auto pMsgSystem = GetSystemManager()->GetMessageSystem();
    pMsgSystem->RegisterFunction(this, Proto::MsgId::MI_NetworkDisconnect, BindFunP1(this, &WorldProxy::HandleNetworkDisconnect));
    pMsgSystem->RegisterFunction(this, Proto::MsgId::MI_Teleport, BindFunP1(this, &WorldProxy::HandleTeleport));
    pMsgSystem->RegisterFunctionFilter<Player>(this, Proto::MsgId::MI_TeleportAfter, BindFunP1(this, &WorldProxy::GetPlayer), BindFunP2(this, &WorldProxy::HandleTeleportAfter));

    pMsgSystem->RegisterFunction(this, Proto::MsgId::MI_BroadcastCreateWorldProxy, BindFunP1(this, &WorldProxy::HandleBroadcastCreateWorldProxy));
    pMsgSystem->RegisterFunctionFilter<Player>(this, Proto::MsgId::S2G_SyncPlayer, BindFunP1(this, &WorldProxy::GetPlayer), BindFunP2(this, &WorldProxy::HandleS2GSyncPlayer));

    // 客户端发送来的协议
    pMsgSystem->RegisterFunctionFilter<Player>(this, Proto::MsgId::C2G_EnterWorld, BindFunP1(this, &WorldProxy::GetPlayer), BindFunP2(this, &WorldProxy::HandleC2GEnterWorld));

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

void WorldProxy::SendPacketToWorld(const Proto::MsgId msgId, Player* pPlayer) const
{
    TagKey tagKey;
    tagKey.AddTag(TagType::Player, pPlayer->GetPlayerSN());
    tagKey.AddTag(TagType::Entity, _sn);
    if (Global::GetInstance()->GetCurAppType() == APP_ALLINONE)
    {
        tagKey.AddTag(TagType::ToWorld, _sn);
    }
    MessageSystemHelp::SendPacket(msgId, &tagKey, APP_SPACE, _spaceAppId);
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

Player* WorldProxy::GetPlayer(NetIdentify* pIdentify)
{
    auto pTags = pIdentify->GetTagKey();
    const auto pTagAccount = pTags->GetTagValue(TagType::Account);
    if (pTagAccount != nullptr)
    {
        auto pPlayerMgr = this->GetComponent<PlayerCollectorComponent>();
        return pPlayerMgr->GetPlayerBySocket(pIdentify->GetSocketKey()->Socket);
    }

    const auto pTagPlayer = pTags->GetTagValue(TagType::Player);
    if (pTagPlayer != nullptr)
    {
        auto pPlayerMgr = this->GetComponent<PlayerCollectorComponent>();
        return pPlayerMgr->GetPlayerBySn(pTagPlayer->KeyInt64);
    }

    return nullptr;
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
    if (!NetworkHelp::IsTcp(pPacket->GetSocketKey()->NetType))
        return;

    LOG_DEBUG("world proxy 1. disconnect." << pPacket);

    TagValue* pTagValue = pPacket->GetTagKey()->GetTagValue(TagType::Account);
    if (pTagValue != nullptr)
    {
        const auto pPlayerCollector = GetComponent<PlayerCollectorComponent>();
        if (pPlayerCollector == nullptr)
            return;

        const auto pPlayer = pPlayerCollector->GetPlayerBySocket(pPacket->GetSocketKey()->Socket);
        if (pPlayer == nullptr)
            return;

        LOG_DEBUG("world proxy 2. disconnect." << pPacket);

        auto pCollector = GetComponent<PlayerCollectorComponent>();
        pCollector->RemovePlayerBySocket(pPacket->GetSocketKey()->Socket);

        SendPacketToWorld(Proto::MsgId::MI_NetworkDisconnect, pPlayer);
    }
    else
    {
        // 可能是space, login, appmgr，dbmgr断线
        auto pTags = pPacket->GetTagKey();
        const auto pTagApp = pTags->GetTagValue(TagType::App);
        if (pTagApp == nullptr)
            return;

        const auto appKey = pTagApp->KeyInt64;
        const auto appType = GetTypeFromAppKey(appKey);
        const auto appId = GetIdFromAppKey(appKey);

        if (appType != APP_SPACE || _spaceAppId != appId)
            return;

        LOG_DEBUG("world proxy 2. disconnect." << pPacket);

        // 玩家需要全部断线
        auto pPlayerCollector = GetComponent<PlayerCollectorComponent>();
        pPlayerCollector->RemoveAllPlayerAndCloseConnect();

        // locator
        auto pWorldLocator = ComponentHelp::GetGlobalEntitySystem()->GetComponent<WorldProxyLocator>();
        pWorldLocator->Remove(_worldId, GetSN());

        // worldproxy 需要销毁
        GetSystemManager()->GetEntitySystem()->RemoveComponent(this);
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

void WorldProxy::HandleTeleportAfter(Player* pPlayer, Packet* pPacket)
{
    auto proto = pPacket->ParseToProto<Proto::TeleportAfter>();
    const auto playerSn = proto.player_sn();

    auto pPlayerMgr = GetComponent<PlayerCollectorComponent>();
    pPlayerMgr->RemovePlayerBySocket(pPlayer->GetSocketKey()->Socket);

    Proto::RemovePlayer protoRs;
    protoRs.set_player_sn(playerSn);
    SendPacketToWorld(Proto::MsgId::G2S_RemovePlayer, protoRs, pPlayer);
}

void WorldProxy::HandleC2GEnterWorld(Player* pPlayer, Packet* pPacket)
{
    auto proto = pPacket->ParseToProto<Proto::EnterWorld>();
    auto worldId = proto.world_id();
    const auto pResMgr = ResourceHelp::GetResourceManager();
    const auto pWorldRes = pResMgr->Worlds->GetResource(worldId);

    if (pWorldRes == nullptr)
        return;

    auto pTeleportComponent = this->GetComponent<WorldComponentTeleport>();
    if (pTeleportComponent->IsTeleporting(pPlayer))
        return;

    // create teleport object
    GetComponent<WorldComponentTeleport>()->CreateTeleportObject(worldId, pPlayer);
}

void WorldProxy::HandleS2GSyncPlayer(Player* pPlayer, Packet* pPacket)
{
    auto proto = pPacket->ParseToProto<Proto::SyncPlayer>();
    pPlayer->ParserFromProto(pPlayer->GetPlayerSN(), proto.player());
    GetComponent<WorldComponentTeleport>()->BroadcastSyncPlayer(pPlayer->GetPlayerSN());
}

void WorldProxy::HandleBroadcastCreateWorldProxy(Packet* pPacket)
{
    auto proto = pPacket->ParseToProto<Proto::BroadcastCreateWorldProxy>();
    GetComponent<WorldComponentTeleport>()->HandleBroadcastCreateWorldProxy(proto.world_id(), proto.world_sn());
}
