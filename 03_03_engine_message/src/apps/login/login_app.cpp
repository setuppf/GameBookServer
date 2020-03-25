#include "login_app.h"
#include <iostream>
#include "test_msg_handler.h"

void LoginApp::InitApp()
{
    AddListenerToThread("127.0.0.1", 2233);
    _pThreadMgr->AddObjToThread(new TestMsgHandler());
}
