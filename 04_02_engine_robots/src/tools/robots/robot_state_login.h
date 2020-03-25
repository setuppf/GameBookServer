#pragma once

#include "robot_state.h"

class RobotStateLoginConnecting : public RobotState {
public:
    DynamicStateCreate(RobotStateLoginConnecting, RobotState_Login_Connecting);

    void OnEnterState() override;
    RobotStateType OnUpdate() override;
};

class RobotStateLoginConnected : public RobotState {
public:
    DynamicStateCreate( RobotStateLoginConnected, RobotState_Login_Connected );

    void OnEnterState( ) override;
};

class RobotStateLoginLogined : public RobotState {
public:
    DynamicStateCreate( RobotStateLoginLogined, RobotState_Login_Logined );
};
