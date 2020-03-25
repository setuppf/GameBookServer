#pragma once

#include "libserver/thread_mgr.h"
#include "libserver/robot_test.h"
#include "libserver/console.h"
#include "libserver/yaml.h"
#include "account.h"

inline void InitializeComponentLogin(ThreadMgr* pThreadMgr)
{
    pThreadMgr->CreateComponent<RobotTest>();
    pThreadMgr->CreateComponent<Account>();
}
