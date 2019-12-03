#include "message_component.h"

MessageComponent::~MessageComponent()
{
    if (_pCallBackFuns != nullptr)
    {
        delete _pCallBackFuns;
        _pCallBackFuns = nullptr;
    }
}

void MessageComponent::Awake(IMessageCallBackFunction* pCallback)
{
    _pCallBackFuns = pCallback;
}

void MessageComponent::BackToPool()
{
    if (_pCallBackFuns != nullptr)
    {
        delete _pCallBackFuns;
        _pCallBackFuns = nullptr;
    }
}

bool MessageComponent::IsFollowMsgId(Packet* packet) const
{
    if (_pCallBackFuns == nullptr)
        return false;

    return _pCallBackFuns->IsFollowMsgId(packet);
}

void MessageComponent::ProcessPacket(Packet* pPacket) const
{
    if (_pCallBackFuns == nullptr)
        return;

    _pCallBackFuns->ProcessPacket(pPacket);
}