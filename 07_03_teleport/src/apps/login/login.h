#pragma once

#include "libserver/thread_mgr.h"
#include "account.h"
#include "redis_login.h"

inline void InitializeComponentLogin(ThreadMgr* pThreadMgr)
{
    pThreadMgr->CreateComponent<Account>();
    pThreadMgr->CreateComponent<RedisLogin>();
}
