#include "thread_collector.h"
#include "message_system.h"
#include "packet.h"

ThreadCollector::ThreadCollector(const ThreadType threadType, int initNum)
{
    _threadType = threadType;
    _index = 0;

    CreateThread(initNum);
}

ThreadCollector::~ThreadCollector()
{
    _threads.Swap();
    auto pList = _threads.GetReaderCache();
    for (auto iter = pList->begin(); iter != pList->end(); ++iter)
    {
        delete (*iter);
    }

    _threads.GetReaderCache()->clear();
}

void ThreadCollector::CreateThread(int num)
{
    for (int i = 0; i < num; i++)
    {
        const auto pThread = new Thread(_threadType);
        _threads.GetAddCache()->push_back(pThread);
    }

    _threads.Swap();
    auto pList = _threads.GetReaderCache();
    for (auto iter = pList->begin(); iter != pList->end(); ++iter)
    {
        (*iter)->Start();
    }
}

void ThreadCollector::Update()
{
    if (_threads.CanSwap())
    {
        auto rs = _threads.Swap();
        for (auto one : rs) {
            delete one;
        }
    }
}

void ThreadCollector::Dispose()
{
    auto pList = _threads.GetReaderCache();
    for (auto iter = pList->begin(); iter != pList->end(); ++iter)
    {
        (*iter)->Dispose();
    }
    _threads.BackToPool();
}

bool ThreadCollector::IsDisposeAll()
{
    auto pList = _threads.GetReaderCache();
    for (auto iter = pList->begin(); iter != pList->end(); ++iter)
    {
        if (!(*iter)->IsDispose())
            return false;
    }

    return true;
}

bool ThreadCollector::IsRunAll()
{
    auto pList = _threads.GetReaderCache();
    for (auto iter = pList->begin(); iter != pList->end(); ++iter)
    {
        if (!(*iter)->IsRun())
            return false;
    }

    return true;
}

bool ThreadCollector::IsStopAll()
{
    auto pList = _threads.GetReaderCache();
    for (auto iter = pList->begin(); iter != pList->end(); ++iter)
    {
        if (!(*iter)->IsStop())
            return false;
    }

    return true;
}


void ThreadCollector::HandlerMessage(Packet* pPacket)
{
    auto pList = _threads.GetReaderCache();
    for (auto iter = pList->begin(); iter != pList->end(); ++iter)
    {
        (*iter)->GetMessageSystem()->AddPacketToList(pPacket);
    }
}

void ThreadCollector::HandlerCreateMessage(Packet* pPacket)
{
    auto vectors = *(_threads.GetReaderCache());
    vectors[_index]->GetMessageSystem()->AddPacketToList(pPacket);
    _index++;
    _index = _index >= vectors.size() ? 0 : _index;
}
