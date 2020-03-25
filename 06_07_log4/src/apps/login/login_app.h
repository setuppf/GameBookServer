#pragma once
#include "libserver/server_app.h"
#include "libserver/app_type_mgr.h"

class LoginApp : public ServerApp {
public:
    explicit LoginApp() : ServerApp(APP_TYPE::APP_LOGIN) { }
    void InitApp() override;
};

