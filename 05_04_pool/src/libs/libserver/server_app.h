#pragma once
#include "disposable.h"
#include "common.h"
#include "thread_mgr.h"
#include "app_type.h"

#if ENGINE_PLATFORM != PLATFORM_WIN32
#include <signal.h>
#else
#include <csignal>
#endif

class ServerApp :public Singleton<ServerApp>, public IDisposable
{
public:
    ServerApp(APP_TYPE appType, int argc, char* argv[]);

    void Initialize();
    void Dispose() override;

    void Run();

    // signal
    static void Signalhandler(int signalValue);

protected:
    ThreadMgr* _pThreadMgr{ nullptr };
    APP_TYPE _appType;

    int _argc;
    char** _argv;
};

