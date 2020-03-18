#include "world_proxy_gather.h"

#include "libserver/message_system_help.h"
#include "libserver/thread_mgr.h"
#include "libserver/message_system.h"
#include "libserver/global.h"

#include <numeric>
#include "libresource/resource_help.h"

void WorldProxyGather::Awake()
{
    AddTimer(0, 10, true, 2, BindFunP0(this, &WorldProxyGather::SyncGameInfo));

    // message
    auto pMsgSystem = GetSystemManager()->GetMessageSystem();    
    pMsgSystem->RegisterFunction(this, Proto::MsgId::MI_WorldProxySyncToGather, BindFunP1(this, &WorldProxyGather::HandleWorldProxySyncToGather));
    pMsgSystem->RegisterFunction(this, Proto::MsgId::MI_CmdWorldProxy, BindFunP1(this, &WorldProxyGather::HandleCmdWorldProxy));
}

void WorldProxyGather::BackToPool()
{
    _maps.clear();
}

void WorldProxyGather::SyncGameInfo()
{
    Proto::AppInfoSync proto;

    const int online = std::accumulate(_maps.begin(), _maps.end(), 0, [](int value, auto pair)
        {
            return value + pair.second.Online;
        });

    proto.set_app_id(Global::GetInstance()->GetCurAppId());
    proto.set_app_type((int)Global::GetInstance()->GetCurAppType());
    proto.set_online(online);

    MessageSystemHelp::SendPacket(Proto::MsgId::MI_AppInfoSync, proto, APP_APPMGR);
}

void WorldProxyGather::HandleCmdWorldProxy(Packet* pPacket)
{
    LOG_DEBUG("------------------------------------");
    LOG_DEBUG("**** world proxy gather ****");

    const auto pResMgr = ResourceHelp::GetResourceManager();
    for (auto one : _maps)
    {
        const auto pWorldRes = pResMgr->Worlds->GetResource(one.second.WorldId);
        std::string worldName = "";
        if (pWorldRes != nullptr)
        {
            worldName = pWorldRes->GetName();
        }
        else
        {
            worldName = "lobby";
        }

        LOG_DEBUG("sn:" << one.first
            << " proxy sn:" << one.second.WorldProxySn
            << " online:" << one.second.Online
            << " name:" << worldName.c_str());
    }
}

void WorldProxyGather::HandleWorldProxySyncToGather(Packet* pPacket)
{
    auto proto = pPacket->ParseToProto<Proto::WorldProxySyncToGather>();
    const uint64 worldSn = proto.world_sn();
    const int worldId = proto.world_id();
    const int online = proto.online();
    const auto proxySn = proto.world_proxy_sn();

    auto iter = _maps.find(worldSn);
    if (iter == _maps.end())
    {
        _maps[worldSn] = WorldProxyInfo();
        _maps[worldSn].WorldId = worldId;
        _maps[worldSn].WorldProxySn = proxySn;
    }

    if (proxySn != _maps[worldSn].WorldProxySn)
    {
        LOG_WARN("error. sync proxy info. proxySn != _maps[worldSn].WorldProxySn");
    }

    _maps[worldSn].Online = online;
}
