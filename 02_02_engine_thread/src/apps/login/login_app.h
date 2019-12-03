#pragma once
#include "libserver/server_app.h"

class LoginApp : public ServerApp {
public:
    explicit LoginApp() : ServerApp(APP_TYPE::APP_LOGIN) { }
    void InitApp() override;
};

