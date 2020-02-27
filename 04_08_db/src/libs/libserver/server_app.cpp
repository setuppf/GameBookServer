#include "common.h"
#include "server_app.h"
#include "object_pool_mgr.h"

#include "console.h"
#include "console_cmd_pool.h"
#include "network_locator.h"
#include "res_path.h"
#include "app_type_mgr.h"
#include "yaml.h"
#include "log4.h"

#if ENGINE_PLATFORM != PLATFORM_WIN32
#include <sys/time.h>
#endif

ServerApp::ServerApp(APP_TYPE appType, int argc, char* argv[])
{
    _appType = appType;
    _argc = argc;
    _argv = argv;
}

void ServerApp::Dispose()
{
    ThreadMgr::DestroyInstance();
    Global::DestroyInstance();
}

void ServerApp::Initialize()
{
    signal(SIGINT, Signalhandler);
    Global::Instance(_appType, 1);

    // 全局数据
    AppTypeMgr::Instance();
    DynamicObjectPoolMgr::Instance();
    ResPath::Instance();
    Log4::Instance(_appType);
    Yaml::Instance();

    ThreadMgr::Instance();
    _pThreadMgr = ThreadMgr::GetInstance();
    UpdateTime();

    // 全局 Component
    _pThreadMgr->AddComponent<NetworkLocator>();
    auto pConsole = _pThreadMgr->AddComponent<Console>();
    pConsole->Register<ConsoleCmdPool>("pool");

    // 创建线程
    const auto pLoginConfig = dynamic_cast<AppConfig*>(Yaml::GetInstance()->GetConfig(_appType));
    for (int i = 0; i < pLoginConfig->ThreadNum; i++)
    {
        _pThreadMgr->CreateThread();
    }

    _pThreadMgr->InitComponent();
    _pThreadMgr->StartAllThread();
}

void ServerApp::Signalhandler(const int signalValue)
{
    switch (signalValue)
    {
#if ENGINE_PLATFORM != PLATFORM_WIN32
    case SIGSTOP:
    case SIGQUIT:
#endif

    case SIGTERM:
    case SIGINT:
        Global::GetInstance()->IsStop = true;
        break;
    }

    std::cout << "\nrecv signal. value:" << signalValue << " Global IsStop::" << Global::GetInstance()->IsStop << std::endl;
}

void ServerApp::Run()
{
    while (!Global::GetInstance()->IsStop)
    {
        UpdateTime();
        _pThreadMgr->Update();
        DynamicObjectPoolMgr::GetInstance()->Update();
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

    // 停止所有线程
    std::cout << "stoping all threads..." << std::endl;
    bool isStop;
    do
    {
        isStop = _pThreadMgr->IsStopAll();
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    } while (!isStop);

    // 释放所有线程资源
    std::cout << "disposing all threads..." << std::endl;

    // 1.子线程资源
    bool isDispose;
    do
    {
        isDispose = _pThreadMgr->IsDisposeAll();
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    } while (!isDispose);

    // 2.主线程资源
    _pThreadMgr->Dispose();

    std::cout << "disposing all pool..." << std::endl;
    DynamicObjectPoolMgr::GetInstance()->Update();
    DynamicObjectPoolMgr::GetInstance()->Dispose();
    DynamicObjectPoolMgr::DestroyInstance();
}

void ServerApp::UpdateTime() const
{
#if ENGINE_PLATFORM != PLATFORM_WIN32
    struct timeval tv;
    gettimeofday(&tv, nullptr);
    Global::GetInstance()->TimeTick = tv.tv_sec * 1000 +  tv.tv_usec * 0.001;
#else
    auto timeValue = std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::system_clock::now());
    Global::GetInstance()->TimeTick = timeValue.time_since_epoch().count();
#endif

//#if ENGINE_PLATFORM != PLATFORM_WIN32
//    auto tt = std::chrono::system_clock::to_time_t(timeValue);
//    struct tm* ptm = localtime(&tt);
//    Global::GetInstance()->YearDay = ptm->tm_yday;
//#else
//    auto tt = std::chrono::system_clock::to_time_t(timeValue);
//    struct tm tm;
//    localtime_s(&tm, &tt);
//    Global::GetInstance()->YearDay = tm.tm_yday;
//#endif
}
