#include "message_callback.h"
#include <iterator>

#include "packet.h"
#include "thread_mgr.h"

void MessageCallBackFunction::RegisterFunction(int msgId, HandleFunction function)
{
     _callbackHandle[msgId] = function;
}

bool MessageCallBackFunction::IsFollowMsgId(Packet* packet)
{
    return _callbackHandle.find(packet->GetMsgId()) != _callbackHandle.end();
}

void MessageCallBackFunction::ProcessPacket(Packet* packet)
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
