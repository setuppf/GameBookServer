#include "message_system.h"
#include <utility>
#include "system_manager.h"
#include "packet.h"
#include "entity_system.h"
#include "component.h"
#include "object_pool_packet.h"
#include "component_help.h"

MessageSystem::MessageSystem(SystemManager* pMgr)
{
    _systemMgr = pMgr;
}

void MessageSystem::Dispose()
{

}

void MessageSystem::AddPacketToList(Packet* pPacket)
{
    std::lock_guard<std::mutex> guard(_packet_lock);
    _cachePackets.GetWriterCache()->emplace_back(pPacket);

    // 进入时 Ref +1
    pPacket->AddRef();
}

void MessageSystem::RegisterFunction(IEntity* obj, int msgId, MsgCallbackFun cbfun)
{
    auto iter = _callbacks.find(msgId);
    if (iter == _callbacks.end())
    {
        _callbacks.insert(std::make_pair(msgId, std::list<IMessageCallBack*>()));
    }

    const auto pCallback = obj->AddComponent<MessageCallBack>(std::move(cbfun));
    _callbacks[msgId].push_back(pCallback);
}

void MessageSystem::RemoveFunction(IComponent* obj)
{
    for (auto iter1 = _callbacks.begin(); iter1 != _callbacks.end(); ++iter1)
    {
        auto subList = iter1->second;
        auto iter2 = std::find_if(subList.begin(), subList.end(), [obj](auto one)
            {
                return one->GetSN() == obj->GetSN();
            });

        if (iter2 == subList.end())
            continue;

        subList.erase(iter2);
    }
}

void MessageSystem::Update(EntitySystem* pEntities)
{
    _packet_lock.lock();
    if (_cachePackets.CanSwap())
    {
        _cachePackets.Swap();
    }
    _packet_lock.unlock();

    if (_cachePackets.GetReaderCache()->empty())
        return;

    auto packetLists = _cachePackets.GetReaderCache();
    for (auto iter = packetLists->begin(); iter != packetLists->end(); ++iter)
    {
        auto pPacket = (*iter);
        const auto finditer = _callbacks.find(pPacket->GetMsgId());
        if (finditer != _callbacks.end())
        {
            auto handleList = finditer->second;
            for (auto pCallBack : handleList)
            {
                pCallBack->ProcessPacket(pPacket);
            }
        }

        // 离开时 Ref - 1
        pPacket->RemoveRef();
    }

    _cachePackets.GetReaderCache()->clear();
}
