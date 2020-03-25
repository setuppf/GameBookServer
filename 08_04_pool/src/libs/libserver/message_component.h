#pragma once

#include "component.h"
#include "message_callback.h"
#include "system.h"

class MessageComponent : public Component<MessageComponent>, public IAwakeFromPoolSystem<IMessageCallBackFunction*>
{
public:
    ~MessageComponent();
    void Awake(IMessageCallBackFunction* pCallback) override;
    void BackToPool() override;

    bool IsFollowMsgId(Packet* packet) const;
    void ProcessPacket(Packet* packet) const;

protected:
    IMessageCallBackFunction* _pCallBackFuns{ nullptr };
};
