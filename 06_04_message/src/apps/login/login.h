#pragma once

#include "libserver/thread_mgr.h"
#include "account.h"

inline void InitializeComponentLogin(ThreadMgr* pThreadMgr)
{
    pThreadMgr->CreateComponent<Account>();
}
