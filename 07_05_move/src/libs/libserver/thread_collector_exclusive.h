#pragma once
#include "thread_collector.h"

class ThreadCollectorExclusive :public ThreadCollector
{
public:
    ThreadCollectorExclusive(ThreadType threadType, int initNum);

    virtual void HandlerMessage(Packet* pPacket) override;
};

