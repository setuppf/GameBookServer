#include "robot_console_login.h"
#include "robot.h"

#include "libserver/common.h"
#include "libserver/thread_mgr.h"

#include <iostream>
#include "global_robots.h"

void RobotConsoleLogin::RegisterHandler()
{
	OnRegisterHandler("-a", BindFunP1(this, &RobotConsoleLogin::HandleLogin));
	OnRegisterHandler("-ex", BindFunP1(this, &RobotConsoleLogin::HandleLoginEx));
	OnRegisterHandler("-clean", BindFunP1(this, &RobotConsoleLogin::HandleLoginClean));
}

void RobotConsoleLogin::HandleHelp()
{
	std::cout << "\t-a account.\t\tlogin by account" << std::endl;
	std::cout << "\t-ex account count.\tbatch login, account is prefix, count as number" << std::endl;
	std::cout << "\t-clean.\t\tclean all logined account." << std::endl;
}

void RobotConsoleLogin::HandleLogin(std::vector<std::string>& params)
{
	if (!CheckParamCnt(params, 1))
		return;

	// 单独登录时，登录到主线程，方便输入cmd
	ThreadMgr::GetInstance()->AddComponent<Robot>(params[0]);
	GlobalRobots::GetInstance()->SetRobotsCount(1);
}

void RobotConsoleLogin::HandleLoginEx(std::vector<std::string>& params) const
{
	if (!CheckParamCnt(params, 2))
		return;

	auto pThreadMgr = ThreadMgr::GetInstance();

	const int count = std::atoi(params[1].c_str());
	if (count == 0)
	{
		std::cout << "input is error. see: -help" << std::endl;
		return;
	}

	for (int i = 1; i <= count; i++)
	{
		const std::string account = params[0] + std::to_string(i);
        pThreadMgr->CreateComponent<Robot>(account);
    }

	GlobalRobots::GetInstance()->SetRobotsCount(count);
}

void RobotConsoleLogin::HandleLoginClean(std::vector<std::string>& params)
{
	std::cout << "close all." << std::endl;
	_threads.clear();
	GlobalRobots::GetInstance()->CleanRobotsCount();
}
