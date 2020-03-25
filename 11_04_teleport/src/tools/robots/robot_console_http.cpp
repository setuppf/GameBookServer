#include "robot_console_http.h"
#include "libserver/thread_mgr.h"
#include "test_http_login.h"

void RobotConsoleHttp::RegisterHandler()
{
	OnRegisterHandler("-check", BindFunP1(this, &RobotConsoleHttp::HandleRequest));
}

void RobotConsoleHttp::HandleHelp()
{
	std::cout << "\t-check test password.\t\tcheck account." << std::endl;
}

void RobotConsoleHttp::HandleRequest(std::vector<std::string>& params)
{
    if (!CheckParamCnt(params, 2))
        return;
    
    ThreadMgr::GetInstance()->CreateComponent<TestHttpLogin>(params[0], params[1]);
}

