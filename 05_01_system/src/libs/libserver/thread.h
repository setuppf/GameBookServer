#pragma once

#include <thread>
#include <list>

#include "system_manager.h"

enum ThreadState
{
    ThreadState_Init,
    ThreadState_Run,
    ThreadState_Stoped,
};

class Thread : public SystemManager
{
public:
    Thread();
    void Start();

    bool IsRun() const;
    bool IsStop() const;
    bool IsDispose();

private:
    ThreadState _state;
    std::thread _thread;
};

