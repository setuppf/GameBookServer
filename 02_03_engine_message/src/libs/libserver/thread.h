#pragma once

#include <thread>
#include <list>

#include "thread_obj.h"
#include "sn_object.h"

class Packet;

class Thread : public SnObject, IDisposable {
public:
    Thread( );
    void AddThreadObj( ThreadObject* _obj );

    void Start( );
    void Stop( );
    void Update( );
    bool IsRun( ) const;
    void Dispose( ) override;
    
    void AddPacket(Packet* pPacket);

private:
    // 本线程的所有对象    
    std::list<ThreadObject*> _objlist;
    std::list<ThreadObject*> _tmpObjs;
    std::mutex _thread_lock;

    bool _isRun;
    std::thread _thread;
};

