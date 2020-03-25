#pragma once

#include "entity_system.h"
#include "yaml.h"
#include "res_path.h"
#include "trace_component.h"

class ComponentHelp
{
public:
    static EntitySystem* GetGlobalEntitySystem();
    static Yaml* GetYaml();
    static ResPath* GetResPath();
    static TraceComponent* GetTraceComponent();

#if ENGINE_PLATFORM != PLATFORM_WIN32    
    static void CatchError(bool bResult); // ¥Ú”°µ±«∞∂—’ª
#endif

};
