#pragma once

#include "libserver/console.h"

#include "app_sync_component.h"
#include "console_cmd_app.h"
#include "create_world_component.h"

inline void InitializeComponentAppMgr(ThreadMgr* pThreadMgr)
{
	pThreadMgr->CreateComponent<AppSyncComponent>();
    pThreadMgr->CreateComponent<CreateWorldComponent>();

    auto pConsole = pThreadMgr->GetEntitySystem()->GetComponent<Console>();
    pConsole->Register<ConsoleCmdApp>("app");
}
