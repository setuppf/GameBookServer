#pragma once

#include "libserver/console.h"

#include "app_sync_component.h"
#include "console_cmd_app.h"

inline void InitializeComponentAppMgr(ThreadMgr* pThreadMgr)
{
	pThreadMgr->CreateComponent<AppSyncComponent>();

    auto pConsole = pThreadMgr->GetEntitySystem()->GetComponent<Console>();
    pConsole->Register<ConsoleCmdApp>("app");
}
