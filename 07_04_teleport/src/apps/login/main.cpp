#include "libserver/common.h"
#include "libserver/server_app.h"
#include "libserver/network_listen.h"
#include "libserver/network_connector.h"

#include "login.h"
#include "libserver/global.h"
#include "libresource/resource_manager.h"

int main(int argc, char* argv[])
{
    const APP_TYPE curAppType = APP_TYPE::APP_LOGIN;
    ServerApp app(curAppType, argc, argv);
    app.Initialize();

    auto pGlobal = Global::GetInstance();

    auto pThreadMgr = ThreadMgr::GetInstance();

    // È«¾Ö
    pThreadMgr->GetEntitySystem()->AddComponent<ResourceManager>();

    InitializeComponentLogin(pThreadMgr);

    // listen
    pThreadMgr->CreateComponent<NetworkListen>(ListenThread, false, (int)pGlobal->GetCurAppType(), (int)pGlobal->GetCurAppId());

    // connector
    pThreadMgr->CreateComponent<NetworkConnector>(ConnectThread, false, (int)NetworkType::TcpConnector, (int)(APP_APPMGR|APP_DB_MGR));
    pThreadMgr->CreateComponent<NetworkConnector>(ConnectThread, false, (int)NetworkType::HttpConnector, (int)0);

    app.Run();
    app.Dispose();

    return 0;
}
