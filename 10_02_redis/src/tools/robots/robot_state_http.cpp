#include "robot_state_http.h"
#include "robot.h"
#include "libserver/message_system_help.h"
#include "libserver/component_help.h"

void RobotStateHttpConnecting::OnEnterState()
{
    const auto pYaml = ComponentHelp::GetYaml();
    const auto pCommonConfig = dynamic_cast<CommonConfig*>(pYaml->GetConfig(APP_APPMGR));
    ObjectKey key{ ObjectKeyType::Account, {0, _pParentObj->GetAccount()} };
    MessageSystemHelp::CreateConnect(NetworkType::HttpConnector, key, pCommonConfig->Ip, pCommonConfig->HttpPort);
}

RobotStateType RobotStateHttpConnecting::OnUpdate()
{
    const auto socketKey = _pParentObj->GetSocketKey();
    if (socketKey.Socket != INVALID_SOCKET && socketKey.NetType == NetworkType::HttpConnector)
    {
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

    const auto pYaml = ComponentHelp::GetYaml();
    const auto pCommonConfig = dynamic_cast<CommonConfig*>(pYaml->GetConfig(APP_APPMGR));
    ObjectKey key{ ObjectKeyType::Account, {0, _pParentObj->GetAccount()} };

    MessageSystemHelp::SendHttpRequest(_pParentObj, pCommonConfig->Ip, pCommonConfig->HttpPort, "/login", nullptr);
}

RobotStateType RobotStateHttpConnected::OnUpdate()
{
    return GetState();
}
