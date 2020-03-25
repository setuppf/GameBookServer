#include "libserver/common.h"
#include "libserver/server_app.h"
#include "libserver/network_listen.h"
#include "libserver/network_connector.h"
#include "libserver/component_help.h"

#include "login.h"

int main(int argc, char* argv[])
{
    const APP_TYPE curAppType = APP_TYPE::APP_LOGIN;
    ServerApp app(curAppType, argc, argv);
    app.Initialize();

    auto pGlobal = Global::GetInstance();

    auto pThreadMgr = ThreadMgr::GetInstance();
    InitializeComponentLogin(pThreadMgr);

    pThreadMgr->CreateThread(ListenThread, 1);
    pThreadMgr->CreateComponent<NetworkListen>(ListenThread, false, (int)pGlobal->GetCurAppType(), pGlobal->GetCurAppId());

    pThreadMgr->CreateThread(ConnectThread, 1);
    pThreadMgr->CreateComponent<NetworkConnector>(ConnectThread, false, (int)APP_TYPE::APP_DB_MGR, 0);
    pThreadMgr->CreateComponent<NetworkConnector>(ConnectThread, false, (int)APP_TYPE::APP_APPMGR, 0);

    app.Run();
    app.Dispose();

    return 0;
}
