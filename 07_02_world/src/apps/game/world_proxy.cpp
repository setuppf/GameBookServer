#include "world_proxy.h"
#include "world_proxy_locator.h"
#include "libserver/component_help.h"

void WorldProxy::Awake(int worldId, uint64 lastWorldSn)
{
    LOG_DEBUG("create world proxy. id:" << worldId);

    _worldId = worldId;

    auto pProxyLocator = ComponentHelp::GetGlobalEntitySystem()->GetComponent<WorldProxyLocator>();
    pProxyLocator->RegisterToLocator(_worldId, GetSN());
}

void WorldProxy::BackToPool()
{
}
