#pragma once
#include "libserver/thread_mgr.h"
#include "world_proxy_gather.h"
#include "lobby.h"
#include "console_cmd_world_proxy.h"
#include "redis_game.h"

inline void InitializeComponentGame(ThreadMgr* pThreadMgr)
{
    pThreadMgr->CreateComponent<Lobby>();
    pThreadMgr->CreateComponent<WorldProxyGather>();
    pThreadMgr->CreateComponent<RedisGame>();

    auto pConsole = pThreadMgr->GetEntitySystem()->GetComponent<Console>();
    pConsole->Register<ConsoleCmdWorldProxy>("wproxy");
}
