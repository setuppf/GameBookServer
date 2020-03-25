#include "global.h"

#if ENGINE_PLATFORM != PLATFORM_WIN32
#include <sys/time.h>
#endif

Global::Global(APP_TYPE appType, int appId)
{
    _appType = appType;
    _appId = appId;
    std::cout << "app type:" << GetAppName(appType) << " id:" << _appId << std::endl;

    UpdateTime();
}

uint64 Global::GenerateSN()
{
    std::lock_guard<std::mutex> guard(_mtx);
    uint64 ret = (TimeTick << 32) + (_serverId << 16) + _snTicket;
    _snTicket += 1;
    return ret;
}

APP_TYPE Global::GetCurAppType() const
{
    return _appType;
}

int Global::GetCurAppId() const
{
    return _appId;
}

void Global::UpdateTime()
{
#if ENGINE_PLATFORM != PLATFORM_WIN32
    struct timeval tv;
    gettimeofday(&tv, nullptr);
    TimeTick = tv.tv_sec * 1000 + tv.tv_usec * 0.001;
#else
    auto timeValue = std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::system_clock::now());
    TimeTick = timeValue.time_since_epoch().count();
#endif

    //#if ENGINE_PLATFORM != PLATFORM_WIN32
    //    auto tt = std::chrono::system_clock::to_time_t(timeValue);
    //    struct tm* ptm = localtime(&tt);
    //    YearDay = ptm->tm_yday;
    //#else
    //    auto tt = std::chrono::system_clock::to_time_t(timeValue);
    //    struct tm tm;
    //    localtime_s(&tm, &tt);
    //    YearDay = tm.tm_yday;
    //#endif
}
