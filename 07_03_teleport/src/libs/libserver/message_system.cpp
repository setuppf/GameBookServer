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
        _callbacks.insert(std::make_pair(msgId, std::map<uint64, IMessageCallBack*>()));
    }

    // 创建 MessageCallBack
    const auto pCallback = _systemMgr->GetEntitySystem()->AddComponent<MessageCallBack>(std::move(cbfun));
    pCallback->SetParent(obj);

    // 将父类的SN，放在key值中，便于查找
    _callbacks[msgId].insert(std::make_pair(obj->GetSN(), pCallback));
}

void MessageSystem::RegisterDefaultFunction(IEntity* obj, MsgCallbackFun cbfun)
{
    // 创建 MessageCallBack
    const auto pCallback = _systemMgr->GetEntitySystem()->AddComponent<MessageCallBack>(std::move(cbfun));
    pCallback->SetParent(obj);

    _defaultCallbacks.insert(std::make_pair(obj->GetSN(), pCallback));
}

void MessageSystem::RemoveFunction(IComponent* obj)
{
    for (auto iter1 = _callbacks.begin(); iter1 != _callbacks.end(); ++iter1)
    {
        auto subList = iter1->second;
        auto iter2 = std::find_if(subList.begin(), subList.end(), [obj](auto pair)
            {
                return pair.second->GetSN() == obj->GetSN();
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
            auto pTagValue = pPacket->GetTagKey()->GetTagValue(TagType::Entity);
            if (pTagValue != nullptr)
            {
                auto entitySn = pTagValue->KeyInt64;
                auto pMsgCallback = handleList[entitySn];
                if (pMsgCallback != nullptr)
                {
                    pMsgCallback->ProcessPacket(pPacket);
                }
            }
            else
            {
                for (auto pair : handleList)
                {
                    pair.second->ProcessPacket(pPacket);
                }
            }
        }
        else
        {
            // 是否有默认处理函数
            const auto pTagValue = pPacket->GetTagKey()->GetTagValue(TagType::Entity);
            if (pTagValue != nullptr)
            {
                // 如果是一个给world的协议，是不可能有默认处理函数的
                // allinone 时 ，world和worldproxy sn相同，会有这个问题
                const auto pTagToWorld = pPacket->GetTagKey()->GetTagValue(TagType::ToWorld);
                if (pTagToWorld == nullptr)
                {
                    auto entitySn = pTagValue->KeyInt64;
                    auto pMsgCallback = _defaultCallbacks[entitySn];
                    if (pMsgCallback != nullptr)
                    {
                        pMsgCallback->ProcessPacket(pPacket);
                    }
                }
            }
        }

        // 离开时 Ref - 1
        pPacket->RemoveRef();
    }

    _cachePackets.GetReaderCache()->clear();
}
