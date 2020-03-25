#include "login_app.h"
#include <iostream>

void LoginApp::InitApp()
{
    AddListenerToThread("127.0.0.1", 2233);
}
