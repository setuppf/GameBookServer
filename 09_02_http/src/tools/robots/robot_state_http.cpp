#include "robot_state_http.h"
#include "robot.h"
#include "libserver/message_system_help.h"
#include "libserver/component_help.h"

void RobotStateHttpConnecting::OnEnterState()
{
    Proto::NetworkConnect protoConn;
    protoConn.set_network_type((int)NetworkType::HttpConnector);

    ObjectKey key{ ObjectKeyType::Account, {0, _pParentObj->GetAccount()} };
    key.SerializeToProto(protoConn.mutable_key());

    const auto pYaml = ComponentHelp::GetYaml();
    const auto pCommonConfig = dynamic_cast<CommonConfig*>(pYaml->GetConfig(APP_APPMGR));
    protoConn.set_ip(pCommonConfig->Ip.c_str());
    protoConn.set_port(pCommonConfig->HttpPort);

    _ip = pCommonConfig->Ip;
    _port = pCommonConfig->HttpPort;
    _method = "/login";

    MessageSystemHelp::DispatchPacket(Proto::MsgId::MI_NetworkConnect, protoConn, nullptr);
}

RobotStateType RobotStateHttpConnecting::OnUpdate()
{
    auto socketKey = _pParentObj->GetSocketKey();
    if (socketKey.Socket != INVALID_SOCKET) 
    {
        if (socketKey.NetType != NetworkType::HttpConnector)
        {
            LOG_ERROR("something is error. socket networkdType:" << GetNetworkTypeName(socketKey.NetType) << " but the state of robot is: RobotStateHttp");
        }

        MessageSystemHelp::SendHttpRequest(_pParentObj, _ip, _port, _method, nullptr);
        return RobotStateType::Http_Connected;
    }

    return GetState();
}

void RobotStateHttpConnected::OnEnterState()
{
#ifdef LOG_TRACE_COMPONENT_OPEN
    std::stringstream traceMsg;
    traceMsg << "http connected.";
    traceMsg << " account:" << _pParentObj->GetAccount();
    traceMsg << " socket:" << _pParentObj->GetSocketKey().Socket;

    ComponentHelp::GetTraceComponent()->TraceAccount(_pParentObj->GetAccount(), _pParentObj->GetSocketKey().Socket);
    ComponentHelp::GetTraceComponent()->Trace(TraceType::Player, _pParentObj->GetSocketKey().Socket, traceMsg.str());
#endif
}

RobotStateType RobotStateHttpConnected::OnUpdate()
{
    return GetState();
}
