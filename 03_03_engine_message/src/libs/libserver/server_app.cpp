#include "common.h"
#include "server_app.h"
#include "network_listen.h"

ServerApp::ServerApp(APP_TYPE  appType)
{
    _appType = appType;

    Global::Instance();
    ThreadMgr::Instance();
    _pThreadMgr = ThreadMgr::GetInstance();
    UpdateTime();

    // 创建线程
    for (int i = 0; i < 3; i++)
    {
        _pThreadMgr->NewThread();
    }
}

ServerApp::~ServerApp()
{
    _pThreadMgr->DestroyInstance();
}

void ServerApp::Dispose()
{
    _pThreadMgr->Dispose();
}

void ServerApp::StartAllThread() const
{
    _pThreadMgr->StartAllThread();
}

void ServerApp::Run() const
{
    bool isRun = true;
    while (isRun)
    {
        UpdateTime();
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        isRun = _pThreadMgr->IsGameLoop();
    }
}

void ServerApp::UpdateTime() const
{
    auto timeValue = std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::system_clock::now());
    Global::GetInstance()->TimeTick = timeValue.time_since_epoch().count();

#if ENGINE_PLATFORM != PLATFORM_WIN32
    auto tt = std::chrono::system_clock::to_time_t(timeValue);
    struct tm* ptm = localtime(&tt);
    Global::GetInstance()->YearDay = ptm->tm_yday;
#else
    auto tt = std::chrono::system_clock::to_time_t(timeValue);
    struct tm tm;
    localtime_s(&tm, &tt);
    Global::GetInstance()->YearDay = tm.tm_yday;
#endif
}

bool ServerApp::AddListenerToThread(const std::string ip, const int port) const
{
    NetworkListen* pListener = new NetworkListen();
    if (!pListener->Listen(ip, port))
    {
        delete pListener;
        return false;
    }

    _pThreadMgr->AddObjToThread(pListener);
    return true;
}
