
#include "libserver/server_app.h"
#include "libserver/network_listen.h"
#include "libserver/component_help.h"
#include "libserver/app_type.h"
#include "libserver/global.h"

#include "appmgr.h"

int main(int argc, char *argv[])
{
    const APP_TYPE curAppType = APP_TYPE::APP_APPMGR;
    ServerApp app(curAppType, argc, argv);
    app.Initialize();

    auto pThreadMgr = ThreadMgr::GetInstance();
    InitializeComponentAppMgr(pThreadMgr);

    const auto pGlobal = Global::GetInstance();
    pThreadMgr->CreateComponent<NetworkListen>(ListenThread, false, (int)pGlobal->GetCurAppType(), pGlobal->GetCurAppId());

    // http listen
    const auto pYaml = ComponentHelp::GetYaml();
    const auto pCommonConfig = pYaml->GetIPEndPoint(pGlobal->GetCurAppType(), pGlobal->GetCurAppId());
    pThreadMgr->CreateComponent<NetworkListen>(ListenThread, false, pCommonConfig->Ip, pCommonConfig->HttpPort);

    app.Run();
    app.Dispose();

	return 0;
}
