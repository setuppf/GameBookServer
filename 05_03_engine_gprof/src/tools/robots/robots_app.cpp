#include "robots_app.h"
#include "robot_mgr.h"
#include "robot_console.h"

void RobotsApp::InitApp()
{
    RobotMgr* mgr = new RobotMgr();
    _pThreadMgr->AddObject(mgr);

    RobotConsole* pConsole = new RobotConsole();
    _pThreadMgr->AddObjToThread(pConsole);
}

