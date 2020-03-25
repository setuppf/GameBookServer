#include "thread_collector.h"
#include "message_system.h"
#include "packet.h"

ThreadCollector::ThreadCollector(const ThreadType threadType, int initNum)
{
    _threadType = threadType;
    _nextThreadSn = 0;

    CreateThread(initNum);
}

void ThreadCollector::CreateThread(int num)
{
    for (int i = 0; i < num; i++)
    {
        const auto pThread = new Thread(_threadType);
        _threads.AddObj(pThread);
    }

    _threads.Swap(nullptr);
    auto pList = _threads.GetReaderCache();
    for (auto iter = pList->begin(); iter != pList->end(); ++iter)
    {
        iter->second->Start();
    }
}

void ThreadCollector::DestroyThread()
{
    auto pList = _threads.GetReaderCache();
    for (auto iter = pList->begin(); iter != pList->end(); ++iter)
    {
        iter->second->DestroyThread();
    }
}

void ThreadCollector::Update()
{
    if (_threads.CanSwap())
    {
        _threads.Swap(nullptr);
    }
}

bool ThreadCollector::IsStopAll()
{
    auto pList = _threads.GetReaderCache();
    for (auto iter = pList->begin(); iter != pList->end(); ++iter)
    {
        if (!iter->second->IsStop())
            return false;
    }

    return true;
}

bool ThreadCollector::IsDestroyAll()
{
    auto pList = _threads.GetReaderCache();
    for (auto iter = pList->begin(); iter != pList->end(); ++iter)
    {
        if (!iter->second->IsDestroy())
            return false;
    }

    return true;
}

void ThreadCollector::Dispose()
{
    _threads.Dispose();
}

void ThreadCollector::HandlerMessage(Packet* pPacket)
{
    auto pList = _threads.GetReaderCache();
    for (auto iter = pList->begin(); iter != pList->end(); ++iter)
    {
        iter->second->GetMessageSystem()->AddPacketToList(pPacket);
    }
}

void ThreadCollector::HandlerCreateMessage(Packet* pPacket, bool isToAllThread)
{
    if (isToAllThread) {
        auto pList = _threads.GetReaderCache();
        for (auto iter = pList->begin(); iter != pList->end(); ++iter)
        {
            iter->second->GetMessageSystem()->AddPacketToList(pPacket);
        }
    }
    else 
    {
        auto objs = _threads.GetReaderCache();
        auto iter = objs->find(_nextThreadSn);
        if (iter == objs->end()) {
            iter = objs->begin();
        }

        iter->second->GetMessageSystem()->AddPacketToList(pPacket);
        ++iter;
        if (iter == objs->end()) {
            iter = objs->begin();
        }

        _nextThreadSn = iter->first;
    }
}
