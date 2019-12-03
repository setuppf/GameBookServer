#pragma once

#include <mutex>

#include "common.h"
#include "thread.h"
#include "singleton.h"
#include <map>

class Packet;
class ThreadObject;

class ThreadMgr :public Singleton<ThreadMgr>
{
public:
    ThreadMgr();
    void StartAllThread();
    bool IsGameLoop();
    void NewThread();
    void AddObjToThread(ThreadObject* obj);

    // message
    void AddPacket(Packet* pPacket);

    void Dispose();

private:
    uint64 _lastThreadSn{ 0 }; // 实现线程对象均分

    std::mutex _thread_lock;
    std::map<uint64, Thread*> _threads;
};

