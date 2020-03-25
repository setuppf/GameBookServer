#include "thread_collector_exclusive.h"
#include "packet.h"
#include "message_system.h"

ThreadCollectorExclusive::ThreadCollectorExclusive(ThreadType threadType, int initNum) :ThreadCollector(threadType, initNum)
{
}

void ThreadCollectorExclusive::HandlerMessage(Packet* pPacket)
{
    if (pPacket->GetMsgId() == Proto::MsgId::MI_CmdShowThreadEntites)
    {
        // 显示指令，每一个线程都执行
        HandlerCreateMessage(pPacket);
    }
    else
    {
        auto vectors = *(_threads.GetReaderCache());
        vectors[_index]->GetMessageSystem()->AddPacketToList(pPacket);
        _index++;
        _index = _index >= vectors.size() ? 0 : _index;
    }
}

void ThreadCollectorExclusive::HandlerCreateMessage(Packet* pPacket)
{
    auto pList = _threads.GetReaderCache();
    for (auto iter = pList->begin(); iter != pList->end(); ++iter)
    {
        (*iter)->GetMessageSystem()->AddPacketToList(pPacket);
    }
}
