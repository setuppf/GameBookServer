#include "libserver/common.h"
#include "libserver/server_app.h"

#include "robot_mgr.h"
#include "robot_console_login.h"
#include "global_robots.h"
#include "robot_console_http.h"
#include "libserver/network_connector.h"
#include "robot_collection.h"

int main(int argc, char* argv[])
{
    GlobalRobots::Instance();

    const APP_TYPE curAppType = APP_TYPE::APP_ROBOT;
    ServerApp app(curAppType, argc, argv);
    app.Initialize();

    const auto pThreadMgr = ThreadMgr::GetInstance();

    auto pConsole = pThreadMgr->GetEntitySystem()->GetComponent<Console>();
    pConsole->Register<RobotConsoleLogin>("login");
    pConsole->Register<RobotConsoleHttp>("http");

    // RobotMgr增加到主线程中
    pThreadMgr->GetEntitySystem()->AddComponent<RobotMgr>();

    // connector
    pThreadMgr->CreateComponent<NetworkConnector>(ConnectThread, false, (int)NetworkType::TcpConnector, (int)0);
    pThreadMgr->CreateComponent<NetworkConnector>(ConnectThread, false, (int)NetworkType::HttpConnector, (int)0);

    // 子线程和主线程都加上 RobotCollection 组件
    pThreadMgr->CreateComponent<RobotCollection>(LogicThread, true);
    pThreadMgr->GetEntitySystem()->AddComponent<RobotCollection>();

    app.Run();
    app.Dispose();
}
