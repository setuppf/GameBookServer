
#include "libserver/common.h"
#include "libserver/server_app.h"
#include "libserver/console.h"
#include "libserver/log4.h"

#include "login/login.h"
#include "dbmgr/dbmgr.h"
#include "libserver/network_listen.h"

int main(int argc, char* argv[])
{
    const APP_TYPE curAppType = APP_TYPE::APP_ALLINONE;

    ServerApp app(curAppType, argc, argv);
    app.Initialize();

    auto pThreadMgr = ThreadMgr::GetInstance();

    // dbmgr
    InitializeComponentDBMgr(pThreadMgr);

    // login
    InitializeComponentLogin(pThreadMgr);

    auto pYaml = Yaml::GetInstance();
    auto pCommonConfig = pYaml->GetIPEndPoint(curAppType);
    pThreadMgr->CreateComponent<NetworkListen>(pCommonConfig->Ip, pCommonConfig->Port);

    app.Run();
    app.Dispose();

    return 0;
}
