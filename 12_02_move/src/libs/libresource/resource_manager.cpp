#include "resource_manager.h"
#include "libserver/log4.h"
#include "libserver/component_help.h"

void ResourceManager::Awake()
{
    const auto pResPath = ComponentHelp::GetResPath();
    Worlds = new ResourceWorldMgr();
    if (!Worlds->Initialize("world", pResPath))
    {
        LOG_ERROR("world txt Initialize. failed.");
    }

    LOG_COLOR(LogColorYellowEx, "all resource loaded.");
}

void ResourceManager::BackToPool()
{

}
