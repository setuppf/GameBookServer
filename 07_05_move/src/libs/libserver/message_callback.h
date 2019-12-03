#pragma once
#include "packet.h"

class IMessageCallBack : public Component<IMessageCallBack>
{
public:
    virtual ~IMessageCallBack() = default;
    virtual bool ProcessPacket(Packet* packet) = 0;
};

using MsgCallbackFun = std::function<void(Packet*)>;
class MessageCallBack :public IMessageCallBack, public IAwakeFromPoolSystem<MsgCallbackFun>
{
public:
    void Awake(MsgCallbackFun fun) override;
    void BackToPool() override;
    virtual bool ProcessPacket(Packet* pPacket) override;

private:
    MsgCallbackFun _handleFunction;
};

template<class T>
class MessageCallBackFilter :public IMessageCallBack, public IAwakeFromPoolSystem<>
{
public:
    void Awake() override {}
    void BackToPool() override
    {
        HandleFunction = nullptr;
        GetFilterObj = nullptr;
    }

    std::function<void(T*, Packet*)> HandleFunction{ nullptr };
    std::function<T * (NetIdentify*)> GetFilterObj{ nullptr };

    virtual bool ProcessPacket(Packet* pPacket) override
    {
        auto pObj = GetFilterObj(pPacket);
        if (pObj == nullptr)
            return false;

#ifdef LOG_TRACE_COMPONENT_OPEN
        const google::protobuf::EnumDescriptor* descriptor = Proto::MsgId_descriptor();
        const auto name = descriptor->FindValueByNumber(pPacket->GetMsgId())->name();

        const auto traceMsg = std::string("process. ")
            .append(" sn:").append(std::to_string(pPacket->GetSN()))
            .append(" msgId:").append(name);
        ComponentHelp::GetTraceComponent()->Trace(TraceType::Packet, pPacket->GetSocketKey()->Socket, traceMsg);
#endif

        HandleFunction(pObj, pPacket);
        return true;
    }
};

