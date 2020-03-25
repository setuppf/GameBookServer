#include "app_type_mgr.h"

AppTypeMgr::AppTypeMgr()
{
	_names.insert(std::make_pair(APP_TYPE::APP_Global, "global"));

	_names.insert(std::make_pair(APP_TYPE::APP_DB_MGR, "dbmgr"));

	_names.insert(std::make_pair(APP_TYPE::APP_GAME_MGR, "gamemgr"));
	_names.insert(std::make_pair(APP_TYPE::APP_SPACE_MGR, "spacemgr"));

	_names.insert(std::make_pair(APP_TYPE::APP_APPMGR, "appmgr"));

	_names.insert(std::make_pair(APP_TYPE::APP_LOGIN, "login"));
	_names.insert(std::make_pair(APP_TYPE::APP_GAME, "game"));
	_names.insert(std::make_pair(APP_TYPE::APP_SPACE, "space"));
	_names.insert(std::make_pair(APP_TYPE::APP_ROBOT, "robot"));

	_names.insert(std::make_pair(APP_TYPE::APP_ALLINONE, "allinone"));
}

std::string AppTypeMgr::GetAppName(const APP_TYPE appType)
{
	const auto iter = _names.find(appType);
	if (iter == _names.end())
		return "";

	return iter->second;
}

