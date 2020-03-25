#pragma once
#include "mysql_connector.h"

#include "libserver/thread_mgr.h"

inline void InitializeComponentDBMgr(ThreadMgr* pThreadMgr)
{
	pThreadMgr->CreateComponent<MysqlConnector>();
}
