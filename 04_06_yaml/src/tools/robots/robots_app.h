#pragma once
#include "libserver/server_app.h"
#include "libserver/app_type_mgr.h"

class RobotsApp : public ServerApp
{
public:
	explicit RobotsApp() : ServerApp(APP_TYPE::APP_ROBOT)
	{
	}
	void InitApp() override;
};

