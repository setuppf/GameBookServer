#pragma once

#include <thread>
#include <list>

#include "sn_object.h"
#include "system_manager.h"
#include "thread_type.h"

enum class ThreadState
{
    Init,
    Run,
    Stop,
    Destroy,
};

class Thread : public SystemManager, public SnObject
{
public:
    Thread(ThreadType threadType);
    ~Thread();

    void Start();
    void DestroyThread();
    void Dispose() override;

    bool IsStop() const;    
    bool IsDestroy()const;

    ThreadType GetThreadType() const { return _threadType; }

private:
    ThreadType _threadType;

    ThreadState _state;
    std::thread _thread;
};

