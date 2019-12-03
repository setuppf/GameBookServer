#pragma once

enum APP_TYPE
{
    // APP_Global 和 APP_None 值相同， APP_Global 只用于读取配置
    APP_None = 0,
    APP_Global = 0,

    APP_DB_MGR = 1,
    APP_GAME_MGR = 1 << 1,
    APP_SPACE_MGR = 1 << 2,

    APP_LOGIN = 1 << 3,
    APP_GAME = 1 << 4,
    APP_SPACE = 1 << 5,
    APP_ROBOT = 1 << 6,

    APP_APPMGR = APP_GAME_MGR | APP_SPACE_MGR,

    APP_ALLINONE = APP_DB_MGR | APP_GAME_MGR | APP_SPACE_MGR | APP_LOGIN | APP_GAME | APP_SPACE,
};

inline const uint64 GetAppKey(int appType, int appId)
{
    return ((uint64)appType << 32) + appId;
}

inline const uint64 GetAppKey(APP_TYPE appType, int appId)
{
    return GetAppKey((int)appType, appId);
}

inline const APP_TYPE GetTypeFromAppKey(uint64 appKey)
{
    return (APP_TYPE)(appKey >> 32);
}

inline const int GetIdFromAppKey(uint64 appKey)
{
    return (int)(appKey & 0x0000FFFF);
}

inline const char* GetAppName(const APP_TYPE appType)
{
    if (appType == APP_TYPE::APP_ALLINONE)
        return "allinone";

    if (appType == APP_TYPE::APP_ROBOT)
        return "robot";

    if (appType == APP_TYPE::APP_SPACE)
        return "space";

    if (appType == APP_TYPE::APP_GAME)
        return "game";

    if (appType == APP_TYPE::APP_LOGIN)
        return "login";

    if (appType == APP_TYPE::APP_DB_MGR)
        return "dbmgr";

    if (appType == APP_TYPE::APP_APPMGR)
        return "appmgr";

    if (appType == APP_TYPE::APP_None)
        return "none";

    return "unknown";
}
