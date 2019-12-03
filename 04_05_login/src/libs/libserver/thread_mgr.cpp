#include "thread_mgr.h"
#include "common.h"
#include <iostream>

ThreadMgr::ThreadMgr()
{
}

void ThreadMgr::StartAllThread()
{
    for (auto iter = _threads.begin(); iter != _threads.end(); ++iter)
    {
        (*iter)->Start();
    }
}

void ThreadMgr::Update()
{
    _create_lock.lock();
    if (_createPackets.CanSwap()) {
        _createPackets.Swap();
    }
    _create_lock.unlock();

    auto pList = _createPackets.GetReaderCache();
    for (auto iter = pList->begin(); iter != pList->end(); ++iter)
    {
        const auto packet = (*iter);
        if (_threadIndex >= _threads.size())
            _threadIndex = 0;

        _threads[_threadIndex]->AddPacketToList(packet);
        _threadIndex++;
    }
    pList->clear();

    EntitySystem::Update();
}

void ThreadMgr::CreateThread()
{
    _threads.emplace_back(new Thread());
}

bool ThreadMgr::IsStopAll()
{
    for (auto iter = _threads.begin(); iter != _threads.end(); ++iter)
    {
        if (!(*iter)->IsStop())
        {
            return false;
        }
    }
    return true;
}

bool ThreadMgr::IsDisposeAll()
{
    for (auto iter = _threads.begin(); iter != _threads.end(); ++iter)
    {
        if (!(*iter)->IsDispose())
        {
            return false;
        }
    }
    return true;
}

void ThreadMgr::Dispose()
{
    EntitySystem::Dispose();

    auto iter = _threads.begin();
    while (iter != _threads.end())
    {
        Thread* pThread = *iter;
        pThread->Dispose();
        delete pThread;
        ++iter;
    }
    _threads.clear();
}

void ThreadMgr::DispatchPacket(Packet* pPacket)
{
    // 主线程
    AddPacketToList(pPacket);

    // 子线程
    for (auto iter = _threads.begin(); iter != _threads.end(); ++iter)
    {
        (*iter)->AddPacketToList(pPacket);
    }
}

