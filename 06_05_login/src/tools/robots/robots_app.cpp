#include "robots_app.h"
#include "robot_mgr.h"
#include "robot_console_login.h"

void RobotsApp::InitApp()
{
    auto pConsole = _pThreadMgr->GetComponent<Console>();
    pConsole->Register<RobotConsoleLogin>("login");

    _pThreadMgr->AddComponent<RobotMgr>();
}
