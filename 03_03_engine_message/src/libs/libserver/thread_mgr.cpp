#include "thread_mgr.h"
#include "common.h"
#include "packet.h"

#include <iostream>

ThreadMgr::ThreadMgr()
{
}

void ThreadMgr::StartAllThread()
{
    auto iter = _threads.begin();
    while (iter != _threads.end())
    {
        iter->second->Start();
        ++iter;
    }
}

bool ThreadMgr::IsGameLoop()
{
    for (auto iter = _threads.begin(); iter != _threads.end(); ++iter)
    {
        if (iter->second->IsRun())
            return true;
    }

    return false;
}

void ThreadMgr::NewThread()
{
    std::lock_guard<std::mutex> guard(_thread_lock);
    auto pThread = new Thread();
    _threads.insert(std::make_pair(pThread->GetSN(), pThread));
}

// 平均加入各线程中
void ThreadMgr::AddObjToThread(ThreadObject* obj)
{
    std::lock_guard<std::mutex> guard(_thread_lock);

    // 在加入之前初始化一下
    if (!obj->Init())
    {
        std::cout << "AddThreadObj Failed. ThreadObject init failed." << std::endl;
        return;
    }

    // 找到上一次的线程	
    auto iter = _threads.begin();
    if (_lastThreadSn > 0)
    {
        iter = _threads.find(_lastThreadSn);
    }

    if (iter == _threads.end())
    {
        // 没有找到，可能没有配线程
        std::cout << "AddThreadObj Failed. no thead." << std::endl;
        return;
    }

    // 取到它的下一个活动线程
    do
    {
        ++iter;
        if (iter == _threads.end())
            iter = _threads.begin();
    } while (!(iter->second->IsRun()));

    auto pThread = iter->second;
    pThread->AddThreadObj(obj);
    _lastThreadSn = pThread->GetSN();
    //std::cout << "add obj to thread.id:" << pThread->GetSN() << std::endl;
}

void ThreadMgr::Dispose()
{
    auto iter = _threads.begin();
    while (iter != _threads.end())
    {
        Thread* pThread = iter->second;
        pThread->Dispose();
        delete pThread;
        ++iter;
    }
}

void ThreadMgr::AddPacket(Packet* pPacket)
{
    std::lock_guard<std::mutex> guard(_thread_lock);
    for (auto iter = _threads.begin(); iter != _threads.end(); ++iter)
    {
        Thread* pThread = iter->second;
        pThread->AddPacket(pPacket);
    }
}