#pragma once

#include <thread>
#include <list>

#include "sn_object.h"
#include "disposable.h"

class Packet;
class ThreadObject;

class ThreadObjectList: public IDisposable
{
public:
    void AddObject(ThreadObject* _obj);
    void Update();
    void AddPacketToList(Packet* pPacket);
    void Dispose() override;

protected:
    // 本线程的所有对象    
    std::list<ThreadObject*> _objlist;
    std::mutex _obj_lock;
};

class Thread : public ThreadObjectList, public SnObject {
public:
    Thread();
    void Start();
    void Stop();
    bool IsRun() const;
   
private:
    bool _isRun;
    std::thread _thread;
};

