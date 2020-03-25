#pragma once

#include "common.h"
#include "util_time.h"
#include "app_type.h"
#include "singleton.h"

#include <mutex>

class Global : public Singleton<Global>
{
public:
    Global(APP_TYPE appType, int appId);
    void UpdateTime();

	uint64 GenerateSN();	// SN = 64位,时间+服务器ID+ticket

	APP_TYPE GetCurAppType() const;
	int GetCurAppId() const;

	int YearDay;
	timeutil::Time TimeTick;

	bool IsStop{ false };

private:
	std::mutex _mtx;
	unsigned short _snTicket{ 1 };
	unsigned int _serverId{ 0 };

	APP_TYPE _appType;
	int _appId{ 0 };
};

