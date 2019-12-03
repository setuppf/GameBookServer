#include "message_callback.h"

void MessageCallBack::Awake(MsgCallbackFun fun)
{
    _handleFunction = fun;
}

void MessageCallBack::BackToPool()
{
    _handleFunction = nullptr;
}

bool MessageCallBack::ProcessPacket(Packet* pPacket)
{
#ifdef LOG_TRACE_COMPONENT_OPEN
    const google::protobuf::EnumDescriptor* descriptor = Proto::MsgId_descriptor();
    const auto name = descriptor->FindValueByNumber(pPacket->GetMsgId())->name();

    const auto traceMsg = std::string("process. ")
        .append(" sn:").append(std::to_string(pPacket->GetSN()))
        .append(" msgId:").append(name);
    ComponentHelp::GetTraceComponent()->Trace(TraceType::Packet, pPacket->GetSocketKey()->Socket, traceMsg);
#endif

    _handleFunction(pPacket);
    return true;
}
