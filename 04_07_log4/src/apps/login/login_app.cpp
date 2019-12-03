#include "login_app.h"
#include "account.h"
#include "libserver/robot_test.h"
#include "libserver/console.h"
#include "libserver/network_listen.h"
#include "libserver/yaml.h"

void LoginApp::InitApp()
{
    auto pYaml = Yaml::GetInstance();
	const auto pLoginConfig = dynamic_cast<LoginConfig*>(pYaml->GetConfig(_appType));

	_pThreadMgr->CreateComponent<NetworkListen>(pLoginConfig->Ip, pLoginConfig->Port);
	_pThreadMgr->CreateComponent<RobotTest>();
	_pThreadMgr->CreateComponent<Account>();
}
