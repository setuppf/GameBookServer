#pragma once

#include <thread>
#include <list>

#include "sn_object.h"
#include "system_manager.h"
#include "thread_type.h"

enum ThreadState
{
    ThreadState_Init,
    ThreadState_Run,
    ThreadState_Stoped,
};

class Thread : public SystemManager, public SnObject
{
public:
    Thread(ThreadType threadType);
    void BackToPool();

    void Start();
    bool IsRun() const;
    bool IsStop() const;
    bool IsDispose();

private:
    ThreadType _threadType;

    ThreadState _state;
    std::thread _thread;
};

