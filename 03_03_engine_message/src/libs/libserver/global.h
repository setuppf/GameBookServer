#pragma once

#include "common.h"
#include "time.h"
#include "singleton.h"

#include <mutex>

class Global : public Singleton<Global> {
public:
	uint64 GenerateSN( );	// SN = 64位,时间+服务器ID+ticket

	int YearDay;
	timeutil::Time TimeTick;

private:
	std::mutex _mtx;
	unsigned int _snTicket{ 1 };
	unsigned int _serverId{ 0 };
};

