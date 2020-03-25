#include "thread_mgr.h"
#include "common.h"
#include "message_system.h"
#include "yaml.h"
#include "log4.h"
#include "res_path.h"
#include "packet.h"
#include "log4_help.h"

#include "network_locator.h"
#include "console.h"
#include "console_cmd_thread.h"

#include "thread_collector_exclusive.h"
#include "global.h"
#include "trace_component.h"
#include "console_cmd_trace.h"
#include "component_help.h"
#include "console_cmd_app.h"
#include "console_efficiency_component.h"
#include "console_cmd_efficiency.h"

ThreadMgr::ThreadMgr()
{
}

void ThreadMgr::InitializeThread()
{
    const auto pGlobal = Global::GetInstance();
    auto pYaml = GetEntitySystem()->GetComponent<Yaml>();
    const auto pConfig = pYaml->GetConfig(pGlobal->GetCurAppType());
    const auto pAppConfig = dynamic_cast<AppConfig*>(pConfig);

    if (pAppConfig->LogicThreadNum > 0)
    {
        CreateThread(LogicThread, pAppConfig->LogicThreadNum);
    }

    if (pAppConfig->MysqlThreadNum > 0)
    {
        CreateThread(MysqlThread, pAppConfig->MysqlThreadNum);
    }

    if (pAppConfig->LogicThreadNum > 0 || pAppConfig->MysqlThreadNum > 0)
    {
        // 如果是多线程，自动创建监听和连接进程
        if (pAppConfig->ListenThreadNum > 0)
            CreateThread(ListenThread, pAppConfig->ListenThreadNum);

        if (pAppConfig->ConnectThreadNum > 0)
            CreateThread(ConnectThread, pAppConfig->ConnectThreadNum);
    }
}

void ThreadMgr::CreateThread(ThreadType iType, int num)
{
    // 不需要创建线程，单线程
    auto pGlobal = Global::GetInstance();
    auto pYaml = GetEntitySystem()->GetComponent<Yaml>();
    const auto pConfig = pYaml->GetConfig(pGlobal->GetCurAppType());
    auto pAppCofig = dynamic_cast<AppConfig*>(pConfig);
    if (pAppCofig->LogicThreadNum == 0 && pAppCofig->MysqlThreadNum == 0)
        return;

    LOG_DEBUG("Initialize thread:" << GetThreadTypeName(iType) << " thread num:" << num);

    auto iter = _threads.find(iType);
    if (iter == _threads.end())
    {
        if (iType == MysqlThread)
            _threads[iType] = new ThreadCollectorExclusive(iType, num);
        else
            _threads[iType] = new ThreadCollector(iType, num);
    }
    else
    {
        _threads[iType]->CreateThread(num);
    }
}

void ThreadMgr::InitializeGlobalComponent(APP_TYPE appType, int appId)
{
    // 全局 Component
    GetEntitySystem()->AddComponent<ResPath>();
    GetEntitySystem()->AddComponent<Log4>(appType);
    GetEntitySystem()->AddComponent<Yaml>();
    GetEntitySystem()->AddComponent<NetworkLocator>();

    auto pConsole = GetEntitySystem()->AddComponent<Console>();
    pConsole->Register<ConsoleCmdThread>("thread");

#if LOG_EFFICIENCY_COMPONENT_OPEN
    pConsole->Register<ConsoleCmdEfficiency>("efficiency");
    GetEntitySystem()->AddComponent<ConsoleEfficiencyComponent>();
#endif

    if ((appType & APP_TYPE::APP_APPMGR) != 0 || (appType & APP_TYPE::APP_GAME) != 0)
        pConsole->Register<ConsoleCmdApp>("app");

#if LOG_TRACE_COMPONENT_OPEN
    GetEntitySystem()->AddComponent<TraceComponent>();
    pConsole->Register<ConsoleCmdTrace>("trace");
#endif

    // 线程程上的基本组件
    InitComponent(ThreadType::MainThread);
}

void ThreadMgr::Update()
{
    UpdateCreatePacket();
    UpdateDispatchPacket();
    SystemManager::Update();
}

void ThreadMgr::UpdateCreatePacket()
{
    _create_lock.lock();
    if (_createPackets.CanSwap()) {
        _createPackets.Swap();
    }
    _create_lock.unlock();

    auto pList = _createPackets.GetReaderCache();
    for (auto iter = pList->begin(); iter != pList->end(); ++iter)
    {
        auto pPacket = (*iter);
        if (!_threads.empty())
        {
            auto pCreateProto = pPacket->ParseToProto<Proto::CreateComponent>();
            auto threadType = (ThreadType)(pCreateProto.thread_type());
            if (_threads.find(threadType) == _threads.end())
            {
                LOG_ERROR("can't find threadtype:" << GetThreadTypeName(threadType));
                continue;
            }

            auto pThreadCollector = _threads[threadType];
            pThreadCollector->HandlerCreateMessage(pPacket, pCreateProto.is_to_all_thread());
        }
        else
        {
            // 单线程
            GetMessageSystem()->AddPacketToList(pPacket);
        }

        pPacket->OpenRef();
    }
    pList->clear();
}

void ThreadMgr::UpdateDispatchPacket()
{
    _packet_lock.lock();
    if (_packets.CanSwap()) {
        _packets.Swap();
    }
    _packet_lock.unlock();

    auto pList = _packets.GetReaderCache();
    for (auto iter = pList->begin(); iter != pList->end(); ++iter)
    {
        auto pPacket = (*iter);

        // 主线程
        GetMessageSystem()->AddPacketToList(pPacket);

        // 子线程
        for (auto iter = _threads.begin(); iter != _threads.end(); ++iter)
        {
            iter->second->HandlerMessage(pPacket);
        }

        pPacket->OpenRef();
    }
    pList->clear();
}

bool ThreadMgr::IsStopAll()
{
    for (auto iter = _threads.begin(); iter != _threads.end(); ++iter)
    {
        if (!iter->second->IsStopAll())
        {
            return false;
        }
    }
    return true;
}

void ThreadMgr::DestroyThread()
{
    for (auto iter = _threads.begin(); iter != _threads.end(); ++iter)
    {
        iter->second->DestroyThread();
    }
}

bool ThreadMgr::IsDestroyAll()
{
    for (auto iter = _threads.begin(); iter != _threads.end(); ++iter)
    {
        if (!iter->second->IsDestroyAll())
        {
            return false;
        }
    }
    return true;
}

void ThreadMgr::Dispose()
{
    SystemManager::Dispose();

    for (auto iter = _threads.begin(); iter != _threads.end(); ++iter)
    {
        auto pObj = iter->second;
        pObj->Dispose();
        delete pObj;
    }

    _threads.clear();
}

void ThreadMgr::DispatchPacket(Packet* pPacket)
{
    std::lock_guard<std::mutex> guard(_packet_lock);
    _packets.GetWriterCache()->emplace_back(pPacket);
}
