#pragma once

#include <thread>
#include <list>

#include "thread_obj.h"
#include "sn_object.h"

class Packet;

class ThreadObjectList
{
public:
    void AddObject(ThreadObject* _obj);
    void Update();
    void AddPacketToList(Packet* pPacket);

protected:
    // 本线程的所有对象    
    std::list<ThreadObject*> _objlist;
    std::mutex _obj_lock;
};

class Thread : public ThreadObjectList, public SnObject, public IDisposable {
public:
    Thread();
    void Start();
    void Stop();
    bool IsRun() const;
    void Dispose() override;
    

private:
    bool _isRun;
    std::thread _thread;
};

