#pragma once

#include <map>
#include <list>
#include <functional>

#include "common.h"
#include "system.h"
#include "cache_swap.h"
#include "socket_object.h"
#include "component_help.h"
#include "packet.h"
#include "message_callback.h"

class IComponent;
class SystemManager;
class Packet;
class EntitySystem;

class MessageSystem :virtual public ISystem<MessageSystem>
{
public:
    MessageSystem(SystemManager* pMgr);
    void Dispose() override;

    void Update(EntitySystem* pEntities) override;
    void AddPacketToList(Packet* pPacket);

    void RegisterFunction(IEntity* obj, int msgId, MsgCallbackFun cbfun);
    void RegisterDefaultFunction(IEntity* obj, MsgCallbackFun cbfun);
    void RemoveFunction(IComponent* obj);

    template<typename T>
    void RegisterFunctionFilter(IEntity* obj, int msgId, std::function<T * (NetIdentify*)> getObj, std::function<void(T*, Packet*)> fun);

    
private:
    // 本线程中的所有待处理包
    std::mutex _packet_lock;
    CacheSwap<Packet> _cachePackets; 

    SystemManager* _systemMgr{ nullptr };

    // message
    // <msgid, <objsn, callback>>
    std::map<int, std::map<uint64, IMessageCallBack*>> _callbacks;

    // 默认处理函数
    // <objsn, callback>
    std::map<uint64, IMessageCallBack*> _defaultCallbacks;
};

template <typename T>
void MessageSystem::RegisterFunctionFilter(IEntity* obj, int msgId, std::function<T * (NetIdentify*)> getObj, std::function<void(T*, Packet*)> fun)
{
    auto iter = _callbacks.find(msgId);
    if (iter == _callbacks.end())
    {
        _callbacks.insert(std::make_pair(msgId, std::map<uint64, IMessageCallBack*>()));
    }

    auto pCallback = _systemMgr->GetEntitySystem()->AddComponent<MessageCallBackFilter<T>>();
    pCallback->GetFilterObj = std::move(getObj);
    pCallback->HandleFunction = std::move(fun);

    pCallback->SetParent(obj);

    _callbacks[msgId].insert(std::make_pair(obj->GetSN(), pCallback));
}
