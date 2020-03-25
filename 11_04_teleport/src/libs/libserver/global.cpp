#include "global.h"

#if ENGINE_PLATFORM != PLATFORM_WIN32
#include <sys/time.h>
#include <uuid/uuid.h>
#else
#include <objbase.h>
#endif

Global::Global(APP_TYPE appType, int appId)
{
    _appType = appType;
    _appId = appId;
    std::cout << "app type:" << GetAppName(appType) << " id:" << _appId << std::endl;

    UpdateTime();
}

int Global::GetAppIdFromSN(uint64 sn)
{
    sn = sn << 38;
    sn = sn >> 38;
    sn = sn >> 16;
    return static_cast<int>(sn);
}

uint64 Global::GenerateSN()
{
    std::lock_guard<std::mutex> guard(_mtx);
    // (38, 10, 16)
    const uint64 ret = ((TimeTick >> 10) << 26) + (_appId << 16) + _snTicket;
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

std::string Global::GenerateUUID()
{
#if ENGINE_PLATFORM == PLATFORM_WIN32
    char buf[64] = { 0 };
    GUID guid;
    if (S_OK == ::CoCreateGuid(&guid))
    {
        _snprintf_s(buf, sizeof(buf)
            , "%08X-%04X-%04x-%02X%02X-%02X%02X%02X%02X%02X%02X"
            , guid.Data1
            , guid.Data2
            , guid.Data3
            , guid.Data4[0], guid.Data4[1]
            , guid.Data4[2], guid.Data4[3], guid.Data4[4], guid.Data4[5]
            , guid.Data4[6], guid.Data4[7]
        );
    }

    std::string tokenkey = buf;
#else
    uuid_t uuid;
    uuid_generate(uuid);

    char key[36];
    uuid_unparse(uuid, key);

    std::string tokenkey = key;
#endif

    return tokenkey;
}
