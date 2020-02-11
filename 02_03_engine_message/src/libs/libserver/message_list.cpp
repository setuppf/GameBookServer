#include "message_list.h"
#include <iterator>

#include "packet.h"

void MessageList::RegisterFunction(int msgId, HandleFunction function)
{
    std::lock_guard<std::mutex> guard(_msgMutex);
    _callbackHandle[msgId] = function;
}

bool MessageList::IsFollowMsgId(int msgId)
{
    std::lock_guard<std::mutex> guard(_msgMutex);
    return _callbackHandle.find(msgId) != _callbackHandle.end();
}

void MessageList::ProcessPacket()
{
    std::list<Packet*> tmpList;
    _msgMutex.lock();
    std::copy(_msgList.begin(), _msgList.end(), std::back_inserter(tmpList));
    _msgList.clear();
    _msgMutex.unlock();

    for (auto packet : tmpList)
    {
        const auto handleIter = _callbackHandle.find(packet->GetMsgId());
        if (handleIter == _callbackHandle.end())
        {
            std::cout << "packet is not hander. msg id;" << packet->GetMsgId() << std::endl;
        }
        else
        {
            handleIter->second(packet);
        }
    }

    tmpList.clear();
}

void MessageList::AddPacket(Packet* pPacket)
{
    std::lock_guard<std::mutex> guard(_msgMutex);
    _msgList.push_back(pPacket);
}
