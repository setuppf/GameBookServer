#include "login_app.h"
#include "account.h"
#include "libserver/robot_test.h"
#include "libserver/console.h"

void LoginApp::InitApp()
{
    AddListenerToThread("127.0.0.1", 2233);

    RobotTest* pTest = new RobotTest();
    _pThreadMgr->AddObjToThread(pTest);

    Account* pAccount = new Account();
    _pThreadMgr->AddObjToThread(pAccount);

    Console* pConsole = new Console();
    _pThreadMgr->AddObjToThread(pConsole);
}
