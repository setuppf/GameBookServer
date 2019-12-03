#pragma once

#include "common.h"
#include "thread.h"
#include "cache_refresh.h"

class Packet;

class ThreadCollector :public IDisposable
{
public:
    ThreadCollector(ThreadType threadType, int initNum);

    void CreateThread(int num);
    void DestroyThread();

    void Update();

    bool IsStopAll();
    bool IsDestroyAll();
    void Dispose() override;

    virtual void HandlerMessage(Packet* pPacket);
    virtual void HandlerCreateMessage(Packet* pPacket);

protected:
    ThreadType _threadType;
    CacheRefresh<Thread> _threads;

    uint64 _nextThreadSn{ 0 };
};

