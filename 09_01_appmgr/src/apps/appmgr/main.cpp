
#include "libserver/server_app.h"
#include "libserver/network_listen.h"
#include "libserver/component_help.h"

#include "appmgr.h"
#include "libserver/app_type.h"

int main(int argc, char *argv[])
{
    const APP_TYPE curAppType = APP_TYPE::APP_APPMGR;
    ServerApp app(curAppType, argc, argv);
    app.Initialize();

    auto pThreadMgr = ThreadMgr::GetInstance();
    InitializeComponentAppMgr(pThreadMgr);

    auto pGlobal = Global::GetInstance();
    pThreadMgr->CreateThread(ListenThread, 1);
    pThreadMgr->CreateComponent<NetworkListen>(ListenThread, false, (int)pGlobal->GetCurAppType(), pGlobal->GetCurAppId());

    app.Run();
    app.Dispose();

	return 0;
}
