#include "robot_console.h"
#include "robot.h"

#include "libserver/common.h"
#include "libserver/thread_mgr.h"

#include <iostream>
#include "global_robots.h"

bool RobotConsole::Init()
{
    if (!Console::Init())
        return false;

    Register<RobotConsoleLogin>("login");
    return true;
}

void RobotConsoleLogin::RegisterHandler()
{
    OnRegisterHandler("-help", BindFunP1(this, &RobotConsoleLogin::HandleHelp));
    OnRegisterHandler("-a", BindFunP1(this, &RobotConsoleLogin::HandleLogin));
    OnRegisterHandler("-ex", BindFunP1(this, &RobotConsoleLogin::HandleLoginEx));
}

void RobotConsoleLogin::HandleHelp(std::vector<std::string>& params)
{
    std::cout << "-a account.\t\tlogin by account" << std::endl;
    std::cout << "-ex account count.\tbatch login, account is prefix, count as number" << std::endl;
}

void RobotConsoleLogin::HandleLogin(std::vector<std::string>& params)
{
    if (!CheckParamCnt(params, 1))
        return;

    GlobalRobots::GetInstance()->SetRobotsCount(1);
    const std::string name = params[0];
    Robot* pRobot = new Robot(name);
    ThreadMgr::GetInstance()->AddObjToThread(pRobot);
}

void RobotConsoleLogin::HandleLoginEx(std::vector<std::string>& params)
{
    if (!CheckParamCnt(params, 2))
        return;

    const int count = std::atoi(params[1].c_str());
    GlobalRobots::GetInstance()->SetRobotsCount(count);
    for (int i = 1; i <= count; i++)
    {
        const std::string account = params[0] + std::to_string(i);
        Robot* pRobot = new Robot(account);
        ThreadMgr::GetInstance()->AddObjToThread(pRobot);
    }
}

