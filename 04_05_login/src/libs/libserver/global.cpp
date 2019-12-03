#include "global.h"

uint64 Global::GenerateSN()
{
	std::lock_guard<std::mutex> guard(_mtx);
	uint64 ret = (TimeTick << 32) + (_serverId << 16) + _snTicket;
	_snTicket += 1;
	return ret;
}

void Global::SetAppInfo(const APP_TYPE appType, const int appId)
{
	_appType = appType;
	_appId = appId;

	std::cout << "app type:" << appType << " id:" << _appId << std::endl;
}

APP_TYPE Global::GetCurAppType() const
{
	return _appType;
}

int Global::GetCurAppId() const
{
	return _appId;
}

