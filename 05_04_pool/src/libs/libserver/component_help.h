#pragma once

#include "entity_system.h"
#include "yaml.h"
#include "res_path.h"

class ComponentHelp
{
public:
    static EntitySystem* GetGlobalEntitySyste();
    static Yaml* GetYaml();
    static ResPath* GetResPath();
};

