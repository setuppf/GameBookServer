#include "libserver/common.h"
#include "libserver/app_type.h"
#include "libserver/server_app.h"
#include "libserver/network_listen.h"
#include "libserver/component_help.h"

#include "dbmgr.h"
#include "mysql_table_update.h"

int main(int argc, char* argv[])
{
    mysql_library_init(0, nullptr, nullptr);

    const APP_TYPE curAppType = APP_TYPE::APP_DB_MGR;
    ServerApp app(curAppType, argc, argv);
    app.Initialize();

    auto pTableUpdateObj = MysqlTableUpdate::Instance();
    pTableUpdateObj->Check();
    pTableUpdateObj->DestroyInstance();

    auto pThreadMgr = ThreadMgr::GetInstance();
    InitializeComponentDBMgr(pThreadMgr);

    auto pYaml = ComponentHelp::GetYaml();
    auto pCommonConfig = pYaml->GetIPEndPoint(curAppType, 0);
    pThreadMgr->CreateThread(ListenThread, 1);
    pThreadMgr->CreateComponent<NetworkListen>(ListenThread, pCommonConfig->Ip, pCommonConfig->Port);
    
    app.Run();
    app.Dispose();

    mysql_library_end();
    return 0;
}
