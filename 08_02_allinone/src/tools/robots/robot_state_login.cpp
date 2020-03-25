#include "robot_state_login.h"
#include "robot.h"

void RobotStateLoginConnectting::OnEnterState()
{
}

RobotStateType RobotStateLoginConnectting::OnUpdate()
{
    if (_pParentObj->IsConnected())
    {
        return RobotState_Login_Connected;
    }

    return GetState();
}

void RobotStateLoginConnectted::OnEnterState() {
    _pParentObj->SendMsgAccountCheck();
}

