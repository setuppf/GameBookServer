#include "world_proxy_locator.h"

#include "libresource/resource_manager.h"

#include "libserver/thread_mgr.h"
#include "libserver/component_help.h"
#include "libresource/resource_help.h"
#include "libserver/message_system.h"
#include "world_proxy.h"

void WorldProxyLocator::Awake()
{
    auto pMsgSystem = GetSystemManager()->GetMessageSystem();
    pMsgSystem->RegisterFunction(this, Proto::MsgId::MI_BroadcastCreateWorld, BindFunP1(this, &WorldProxyLocator::HandleBroadcastCreateWorld));
}

void WorldProxyLocator::BackToPool()
{
    std::lock_guard<std::mutex> guard(_lock);
    _publics.clear();
    _worlds.clear();
}

void WorldProxyLocator::RegisterToLocator(int worldId, const uint64 worldSn)
{
    std::lock_guard<std::mutex> guard(_lock);

    auto pResMgr = ResourceHelp::GetResourceManager();
    const auto mapConfig = pResMgr->Worlds->GetResource(worldId);
    if (mapConfig->IsType(ResourceWorldType::Public))
    {
        if (_publics.find(worldId) != _publics.end())
        {
            LOG_ERROR(" WorldProxyLocator. find same key. worldId:" << worldId);
        }

        _publics[worldId] = worldSn;
    }

    _worlds.emplace(worldSn);
}

void WorldProxyLocator::Remove(const int worldId, const uint64 worldSn)
{
    std::lock_guard<std::mutex> guard(_lock);
    _publics.erase(worldId);
    _worlds.erase(worldSn);
}

bool WorldProxyLocator::IsExistDungeon(const uint64 mspSn)
{
    std::lock_guard<std::mutex> guard(_lock);
    return _worlds.find(mspSn) != _worlds.end();
}

uint64 WorldProxyLocator::GetWorldSnById(const int worldId)
{
    std::lock_guard<std::mutex> guard(_lock);
    const auto iter = _publics.find(worldId);
    if (iter == _publics.end())
        return INVALID_ID;

    return iter->second;
}

void WorldProxyLocator::HandleBroadcastCreateWorld(Packet* pPacket)
{
    std::lock_guard<std::mutex> guard(_lock);
    auto proto = pPacket->ParseToProto<Proto::BroadcastCreateWorld>();
    const int worldId = proto.world_id();
    const auto worldSn = proto.world_sn();
    const uint64 lastWorldSn = proto.last_world_sn();

    const auto worldCfg = ResourceHelp::GetResourceManager()->Worlds->GetResource(worldId);
    if (worldCfg == nullptr)
    {
        LOG_ERROR("WorldProxyLocator::HandleBroadcastCreateWorld. can't find worldId:" << worldId);
        return;
    }

    if (worldCfg->IsType(ResourceWorldType::Public))
    {
        if (_publics.find(worldId) != _publics.end())
        {
            LOG_ERROR(" WorldLocator. find same key. worldId:" << worldId);
            return;
        }

        ThreadMgr::GetInstance()->CreateComponentWithSn<WorldProxy>(worldSn, worldId, lastWorldSn);
    }
    else
    {
        if (_worlds.find(lastWorldSn) == _worlds.end())
        {
            LOG_ERROR("can't find request world. world sn:" << lastWorldSn);
            return;
        }

        ThreadMgr::GetInstance()->CreateComponentWithSn<WorldProxy>(worldSn, worldId, lastWorldSn);
    }
}
