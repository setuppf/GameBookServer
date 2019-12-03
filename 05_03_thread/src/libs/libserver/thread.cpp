#include "thread.h"
#include "global.h"
#include "entity_system.h"
#include "log4.h"

#include <iterator>

Thread::Thread(ThreadType threadType)
{
    _state = ThreadState_Init;
    _threadType = threadType;
}

void Thread::BackToPool()
{
}

void Thread::Start()
{
    _thread = std::thread([this]()
        {
            InitComponent(_threadType);
            _state = ThreadState_Run;
            while (!Global::GetInstance()->IsStop)
            {
                Update();
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
            }

            const auto theadId = _thread.get_id();
            std::cout << "close thread [1/2]. thread id:" << theadId << std::endl;

            Dispose();
            log4cplus::threadCleanup();
            _state = ThreadState_Stoped;
        });
}

bool Thread::IsRun() const
{
    return _state == ThreadState_Run;
}

bool Thread::IsStop() const
{
    return _state == ThreadState_Stoped;
}

bool Thread::IsDispose()
{
    if (_thread.joinable())
    {
        const auto theadId = _thread.get_id();
        _thread.join();
        std::cout << "close thread [2/2]. thread id:" << theadId << std::endl;
        return true;
    }

    return false;
}

