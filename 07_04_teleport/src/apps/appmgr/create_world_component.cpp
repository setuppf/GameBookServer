#include "create_world_component.h"
#include "libresource/resource_manager.h"
#include "libserver/message_system_help.h"
#include "libserver/message_system.h"
#include "libresource/resource_help.h"
#include "libserver/network_help.h"

void CreateWorldComponent::Awake()
{
    // message
    auto pMsgSystem = GetSystemManager()->GetMessageSystem();
    pMsgSystem->RegisterFunction(this, Proto::MsgId::MI_CmdCreate, BindFunP1(this, &CreateWorldComponent::HandleCmdCreate));

    pMsgSystem->RegisterFunction(this, Proto::MsgId::MI_AppInfoSync, BindFunP1(this, &CreateWorldComponent::HandleAppInfoSync));
    pMsgSystem->RegisterFunction(this, Proto::MsgId::MI_NetworkDisconnect, BindFunP1(this, &CreateWorldComponent::HandleNetworkDisconnect));

    pMsgSystem->RegisterFunction(this, Proto::MsgId::G2M_QueryWorld, BindFunP1(this, &CreateWorldComponent::HandleQueryWorld));

    pMsgSystem->RegisterFunction(this, Proto::MsgId::G2M_RequestWorld, BindFunP1(this, &CreateWorldComponent::HandleRequestWorld));
    pMsgSystem->RegisterFunction(this, Proto::MsgId::MI_BroadcastCreateWorld, BindFunP1(this, &CreateWorldComponent::HandleBroadcastCreateWorld));
}

void CreateWorldComponent::BackToPool()
{
    _creating.clear();
    _created.clear();
}

void CreateWorldComponent::HandleAppInfoSync(Packet* pPacket)
{
    AppInfoSyncHandle(pPacket);
}

int CreateWorldComponent::ReqCreateWorld(int worldId)
{
    AppInfo appInfo;
    if (!GetOneApp(APP_SPACE, &appInfo))
    {
        LOG_ERROR("appmgr recv create map. but no space process. map id:" << worldId);
        return INVALID_ID;
    }
    else
    {
        Proto::CreateWorld protoCreate;
        protoCreate.set_world_id(worldId);
        protoCreate.set_last_world_sn(0);
        protoCreate.set_game_app_id(0);
        MessageSystemHelp::SendPacket(Proto::MsgId::G2S_CreateWorld, protoCreate, APP_SPACE, appInfo.AppId);
        return appInfo.AppId;
    }
}

void CreateWorldComponent::HandleCmdCreate(Packet* pPacket)
{
    LOG_DEBUG("------------------------------------");
    LOG_DEBUG("**** public world ****");
    auto pResMgr = ResourceHelp::GetResourceManager();
    for (auto pair : _created)
    {
        LOG_DEBUG("id:" << pair.first
            << " world sn:" << pair.second
            << " name:" << pResMgr->Worlds->GetResource(pair.first)->GetName().c_str());
    }

    LOG_DEBUG("**** dungeons world ****");
    for (auto pair : _dungeons)
    {
        LOG_DEBUG("id:" << pair.second 
            << " world sn:" << pair.first
            << " name:" << pResMgr->Worlds->GetResource(pair.second)->GetName().c_str());
    }

    LOG_DEBUG("**** createing world, count:" << _creating.size());
}

void CreateWorldComponent::HandleNetworkDisconnect(Packet* pPacket)
{
    if (!NetworkHelp::IsTcp(pPacket->GetSocketKey()->NetType))
        return;

    SyncComponent::HandleNetworkDisconnect(pPacket);

    auto pTags = pPacket->GetTagKey();
    const auto pTagApp = pTags->GetTagValue(TagType::App);
    if (pTagApp == nullptr)
        return;

    auto appId = GetIdFromAppKey(pTagApp->KeyInt64);

    // 断线的Space上是否有正在创建的地图
    do
    {
        auto iterCreating = std::find_if(_creating.begin(), _creating.end(), [&appId](auto pair)
            {
                return pair.second == appId;
            });

        if (iterCreating == _creating.end())
            break;

        // 正在创建的时候，space进程断开了，另找进程创建
        auto worldId = iterCreating->first;
        _creating.erase(iterCreating);

        ReqCreateWorld(worldId);

    } while (true);

    // 断线的Space上有已创建的公共地图，全部删除
    do
    {
        auto iterCreated = std::find_if(_created.begin(), _created.end(), [&appId](auto pair)
            {
                return Global::GetAppIdFromSN(pair.second) == appId;
            });


        if (iterCreated == _created.end())
            break;

        _created.erase(iterCreated);
    } while (true);


    // 断线的Space上创建的副本地图
    do
    {
        const auto iter = std::find_if(_dungeons.begin(), _dungeons.end(), [&appId](auto pair)
            {
                return Global::GetAppIdFromSN(pair.first) == appId;
            });

        if (iter == _dungeons.end())
            break;

        _dungeons.erase(iter);
    } while (true);
}

void CreateWorldComponent::HandleRequestWorld(Packet* pPacket)
{
    auto proto = pPacket->ParseToProto<Proto::RequestWorld>();

    auto worldId = proto.world_id();
    auto pResMgr = ResourceHelp::GetResourceManager();
    const auto mapConfig = pResMgr->Worlds->GetResource(worldId);
    if (mapConfig == nullptr)
    {
        LOG_ERROR("can't find map config. id:" << worldId);
        return;
    }

    if (!mapConfig->IsType(ResourceWorldType::Public))
    {
        LOG_ERROR("appmgr recv create dungean map. map id:" << worldId);
        return;
    }

    // 正在创建中，等待
    const auto iter = _creating.find(worldId);
    if (iter != _creating.end())
        return;

    // 已经被创建了
    const auto iter2 = _created.find(worldId);
    if (iter2 != _created.end()) 
    {
        Proto::BroadcastCreateWorld protoRs;
        protoRs.set_world_id(worldId);
        protoRs.set_world_sn(iter2->second);
        protoRs.set_last_world_sn(0);
        MessageSystemHelp::SendPacket(Proto::MI_BroadcastCreateWorld, protoRs, pPacket);
        return;
    }

    const int appId = ReqCreateWorld(worldId);
    if (appId != INVALID_ID)
        _creating[worldId] = appId;
}

void CreateWorldComponent::HandleQueryWorld(Packet* pPacket)
{
    auto proto = pPacket->ParseToProto<Proto::QueryWorld>();
    const auto worldSn = proto.world_sn();

    const auto iter = _dungeons.find(worldSn);
    if (iter == _dungeons.end()) 
    {
        Proto::QueryWorldRs protoQueryRs;
        protoQueryRs.set_world_sn(worldSn);
        protoQueryRs.set_return_code(Proto::QueryWorldRs::QueryWorld_Failed);
        MessageSystemHelp::SendPacket(Proto::MsgId::G2M_QueryWorldRs, protoQueryRs, pPacket);
    }
    else 
    {
        Proto::BroadcastCreateWorld protoRs;
        protoRs.set_world_id(iter->second);
        protoRs.set_world_sn(iter->first);
        protoRs.set_last_world_sn(proto.last_world_sn());
        MessageSystemHelp::SendPacket(Proto::MI_BroadcastCreateWorld, protoRs, pPacket);
    }
}

void CreateWorldComponent::HandleBroadcastCreateWorld(Packet* pPacket)
{
    auto proto = pPacket->ParseToProto<Proto::BroadcastCreateWorld>();
    const auto worldId = proto.world_id();
    const auto worldSn = proto.world_sn();

    const auto pResMgr = ResourceHelp::GetResourceManager();
    const auto pWorldRes = pResMgr->Worlds->GetResource(worldId);
    if (pWorldRes->IsType(ResourceWorldType::Public))
    {
        _created[worldId] = proto.world_sn();
        _creating.erase(worldId);
    }
    else
    {
        _dungeons[worldSn] = worldId;
    }
}
