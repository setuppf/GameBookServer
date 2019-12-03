#include "resource_help.h"
#include "libserver/thread_mgr.h"

ResourceManager* ResourceHelp::GetResourceManager()
{
    return ThreadMgr::GetInstance()->GetEntitySystem()->GetComponent<ResourceManager>();
}
