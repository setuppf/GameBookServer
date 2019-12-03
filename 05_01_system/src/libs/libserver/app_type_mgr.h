#pragma once
#include <map>
#include "singleton.h"

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

    APP_ALL = APP_DB_MGR | APP_GAME_MGR | APP_SPACE_MGR | APP_LOGIN | APP_GAME | APP_SPACE,
};

class AppTypeMgr : public Singleton<AppTypeMgr>
{
public:
    AppTypeMgr();
	std::string GetAppName(APP_TYPE appType);
	
private:
	std::map<APP_TYPE, std::string> _names;
};



