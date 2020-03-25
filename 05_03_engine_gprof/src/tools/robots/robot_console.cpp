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
    OnRegisterHandler("-help", BindFunP2(this, &RobotConsoleLogin::HandleHelp));
    OnRegisterHandler("-a", BindFunP2(this, &RobotConsoleLogin::HandleLogin));
    OnRegisterHandler("-ex", BindFunP2(this, &RobotConsoleLogin::HandleLoginEx));
}

void RobotConsoleLogin::HandleHelp(std::string p1, std::string p2)
{
    std::cout << "-a account.\t\tlogin by account" << std::endl;
    std::cout << "-ex account count.\tbatch login, account is prefix, count as number" << std::endl;
}

void RobotConsoleLogin::HandleLogin(std::string p1, std::string p2)
{
    //std::cout << "login. account:" << p1.c_str() << std::endl;
    GlobalRobots::GetInstance()->SetRobotsCount(1);
    Robot* pRobot = new Robot(p1);
    ThreadMgr::GetInstance()->AddObjToThread(pRobot);
}

void RobotConsoleLogin::HandleLoginEx(std::string p1, std::string p2)
{
    int count = std::atoi(p2.c_str());
    GlobalRobots::GetInstance()->SetRobotsCount(count);
    for (int i = 1; i <= count; i++)
    {
        std::string account = p1 + std::to_string(i);
        Robot* pRobot = new Robot(account);
        ThreadMgr::GetInstance()->AddObjToThread(pRobot);
    }
}

