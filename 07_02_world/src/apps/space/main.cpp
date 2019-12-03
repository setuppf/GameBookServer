#include "libserver/common.h"
#include "libserver/server_app.h"
#include "libserver/global.h"
#include "libserver/network_listen.h"

#include "space.h"
#include "libresource/resource_manager.h"
#include "libserver/network_connector.h"

int main(int argc, char *argv[])
{
    const APP_TYPE curAppType = APP_TYPE::APP_SPACE;
    ServerApp app(curAppType, argc, argv);
    app.Initialize();

    auto pThreadMgr = ThreadMgr::GetInstance();

    // È«¾Ö
    pThreadMgr->GetEntitySystem()->AddComponent<ResourceManager>();

    InitializeComponentSpace(pThreadMgr);

    const auto pGlobal = Global::GetInstance();
    pThreadMgr->CreateComponent<NetworkListen>(ListenThread, false, (int)pGlobal->GetCurAppType(), (int)pGlobal->GetCurAppId());

    // connector
    pThreadMgr->CreateComponent<NetworkConnector>(ConnectThread,false,(int)NetworkType::TcpConnector, (int)(APP_APPMGR));

    app.Run();
    app.Dispose();

    return 0;
}
