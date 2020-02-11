#pragma once
#include <mutex>
#include <map>
#include <list>
#include <functional>

class Packet;

typedef std::function<void(Packet*)> HandleFunction;

class MessageList
{
public:
    void RegisterFunction(int msgId, HandleFunction function);
    bool IsFollowMsgId(int msgId);
    void ProcessPacket();
    void AddPacket(Packet* pPacket);

protected:
    std::mutex _msgMutex;
    std::list<Packet*> _msgList;
    std::map<int, HandleFunction> _callbackHandle;
};
