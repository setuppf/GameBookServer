#pragma once

#include "app_sync_component.h"
#include "create_world_component.h"
#include "console_cmd_create.h"

inline void InitializeComponentAppMgr(ThreadMgr* pThreadMgr)
{
	pThreadMgr->CreateComponent<AppSyncComponent>();
    pThreadMgr->CreateComponent<CreateWorldComponent>();

    auto pConsole = pThreadMgr->GetEntitySystem()->GetComponent<Console>();
    pConsole->Register<ConsoleCmdCreate>("create");
}
