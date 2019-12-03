#include "component_help.h"
#include "thread_mgr.h"

#if ENGINE_PLATFORM != PLATFORM_WIN32
#include <execinfo.h>
#endif

EntitySystem* ComponentHelp::GetGlobalEntitySystem()
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

TraceComponent* ComponentHelp::GetTraceComponent()
{
    return ThreadMgr::GetInstance()->GetEntitySystem()->GetComponent<TraceComponent>();
}

#if ENGINE_PLATFORM != PLATFORM_WIN32
void ComponentHelp::CatchError(bool bResult)
{
    if (bResult)
        return;

    void* array[10];
    size_t size;
    char** strings;
    size_t i;

    size = backtrace(array, 10);
    strings = backtrace_symbols(array, size);

    std::stringstream ss;
    ss << "Obtained " << size << " stack frames.\n";

    for (i = 0; i < size; i++)
        ss << strings[i] << "\n";
        
    LOG_ERROR(ss.str().c_str());
    free(strings);
}
#endif