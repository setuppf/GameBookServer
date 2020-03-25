#include "libserver/common.h"
#include "libserver/server_app.h"
#include "libserver/global.h"
#include "libserver/network_listen.h"

#include "space.h"

int main(int argc, char *argv[])
{
    const APP_TYPE curAppType = APP_TYPE::APP_SPACE;
    ServerApp app(curAppType, argc, argv);
    app.Initialize();

    auto pThreadMgr = ThreadMgr::GetInstance();
    InitializeComponentSpace(pThreadMgr);

    const auto pGlobal = Global::GetInstance();
    pThreadMgr->CreateComponent<NetworkListen>(ListenThread, false, (int)pGlobal->GetCurAppType(), pGlobal->GetCurAppId());
    
    app.Run();
    app.Dispose();

    return 0;
}
