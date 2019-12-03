#include "thread_mgr.h"
#include "common.h"
#include "message_system.h"

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
        if (_threads.size() > 0)
        {
            if (_threadIndex >= _threads.size())
                _threadIndex = 0;

            _threads[_threadIndex]->GetMessageSystem()->AddPacketToList(packet);
            _threadIndex++;
        }
        else
        {
            // 单线程
            GetMessageSystem()->AddPacketToList(packet);
        }
    }
    pList->clear();

    SystemManager::Update();
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
    SystemManager::Dispose();

    auto iter = _threads.begin();
    while (iter != _threads.end())
    {
        Thread* pThread = *iter;
        delete pThread;
        ++iter;
    }
    _threads.clear();
}

void ThreadMgr::DispatchPacket(Packet* pPacket)
{
    // 主线程
    GetMessageSystem()->AddPacketToList(pPacket);

    // 子线程
    for (auto iter = _threads.begin(); iter != _threads.end(); ++iter)
    {
        (*iter)->GetMessageSystem()->AddPacketToList(pPacket);
    }
}

