#include "libserver/common.h"
#include "libserver/server_app.h"
#include "game.h"
#include "libserver/global.h"
#include "libserver/network_listen.h"
#include "libserver/network_connector.h"
#include "libresource/resource_manager.h"

int main(int argc, char* argv[])
{
    const APP_TYPE curAppType = APP_TYPE::APP_GAME;
    ServerApp app(curAppType, argc, argv);
    app.Initialize();

    auto pThreadMgr = ThreadMgr::GetInstance();

    // È«¾Ö
    pThreadMgr->GetEntitySystem()->AddComponent<ResourceManager>();

    InitializeComponentGame(pThreadMgr);

    const auto pGlobal = Global::GetInstance();
    pThreadMgr->CreateComponent<NetworkListen>(ListenThread, false, (int)pGlobal->GetCurAppType(), pGlobal->GetCurAppId());

    // connector
    pThreadMgr->CreateComponent<NetworkConnector>(
        ConnectThread,
        false,
        (int)NetworkType::TcpConnector, (int)(APP_APPMGR | APP_DB_MGR | APP_SPACE));

    app.Run();
    app.Dispose();

    return 0;
}
