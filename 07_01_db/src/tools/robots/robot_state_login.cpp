#include "robot_state_login.h"
#include "robot.h"

void RobotStateLoginConnecting::OnEnterState()
{
}

RobotStateType RobotStateLoginConnecting::OnUpdate()
{
    if (_pParentObj->IsConnected())
    {
        return RobotState_Login_Connected;
    }

    return GetState();
}

void RobotStateLoginConnected::OnEnterState() {
    _pParentObj->SendMsgAccountCheck();
}

