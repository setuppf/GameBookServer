#include "robot_state_login.h"
#include "robot.h"
#include "libserver/message_system_help.h"
#include "libserver/component_help.h"
#include "robot_component_login.h"

void RobotStateLoginConnecting::OnEnterState()
{
    _pParentObj->NetworkDisconnect();

    TagValue tagValue{ _pParentObj->GetAccount(), 0L };
    const auto loginObj = _pParentObj->GetComponent<RobotComponentLogin>();
    MessageSystemHelp::CreateConnect(NetworkType::TcpConnector, TagType::Account, tagValue, loginObj->GetLoginIp(), loginObj->GetLoginPort());
}

RobotStateType RobotStateLoginConnecting::OnUpdate()
{
    const auto socketKey = _pParentObj->GetSocketKey();
    if (socketKey->Socket != INVALID_SOCKET && socketKey->NetType == NetworkType::TcpConnector)
    {
        return RobotStateType::Login_Connected;
    }

    return GetState();
}

void RobotStateLoginConnected::OnEnterState()
{
    Proto::AccountCheck accountCheck;
    accountCheck.set_account(_pParentObj->GetAccount().c_str());
    accountCheck.set_password("e10adc3949ba59abbe56e057f20f883e");
    MessageSystemHelp::SendPacket(Proto::MsgId::C2L_AccountCheck, accountCheck, _pParentObj);

#ifdef LOG_TRACE_COMPONENT_OPEN
    std::stringstream traceMsg;
    traceMsg << "send check account.";
    traceMsg << " account:" << _pParentObj->GetAccount();
    traceMsg << " socket:" << _pParentObj->GetSocketKey()->Socket;

    ComponentHelp::GetTraceComponent()->TraceAccount(_pParentObj->GetAccount(), _pParentObj->GetSocketKey()->Socket);
    ComponentHelp::GetTraceComponent()->Trace(TraceType::Player, _pParentObj->GetSocketKey()->Socket, traceMsg.str());
#endif
}

RobotStateType RobotStateLoginConnected::OnUpdate()
{
    return GetState();
}

RobotStateType RobotStateLoginLogined::OnUpdate()
{
    return GetState();
}

