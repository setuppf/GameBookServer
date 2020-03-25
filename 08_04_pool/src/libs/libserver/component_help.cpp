#include "component_help.h"
#include "thread_mgr.h"

EntitySystem* ComponentHelp::GetGlobalEntitySyste()
{
    return ThreadMgr::GetInstance()->GetEntitySystem();
}

Yaml* ComponentHelp::GetYaml()
{
    return ThreadMgr::GetInstance()->GetEntitySystem()->GetComponent<Yaml>();
}

ResPath* ComponentHelp::GetResPath()
{
    return ThreadMgr::GetInstance()->GetEntitySystem()->GetComponent<ResPath>();
}


