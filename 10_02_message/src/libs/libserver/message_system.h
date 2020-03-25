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
    void RemoveFunction(IComponent* obj);
    
    template<typename T>
    void RegisterFunctionFilter(IEntity* obj, int msgId, std::function<T * (NetworkIdentify*)> pGetObj, std::function<void(T*, Packet*)> pCallBack);

private:
    // 本线程中的所有待处理包
    std::mutex _packet_lock;
    CacheSwap<Packet> _cachePackets;

    SystemManager* _systemMgr{ nullptr };

    // message
    std::map<int, std::list<IMessageCallBack*>> _callbacks;
};

template <typename T>
void MessageSystem::RegisterFunctionFilter(IEntity* obj, int msgId, std::function<T * (NetworkIdentify*)> getObj, std::function<void(T*, Packet*)> fun)
{
    auto iter = _callbacks.find(msgId);
    if (iter == _callbacks.end())
    {
        _callbacks.insert(std::make_pair(msgId, std::list<IMessageCallBack*>()));
    }

    auto pCallback = obj->AddComponent<MessageCallBackFilter<T>>();
    pCallback->GetFilterObj = std::move(getObj);
    pCallback->HandleFunction = std::move(fun);
    _callbacks[msgId].push_back(pCallback);
}
