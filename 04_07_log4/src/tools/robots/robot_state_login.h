#pragma once

#include "robot_state.h"

class RobotStateLoginConnectting : public RobotState {
public:
    DynamicStateCreate(RobotStateLoginConnectting, RobotState_Login_Connectting);

    void OnEnterState() override;
    RobotStateType OnUpdate() override;
};

class RobotStateLoginConnectted : public RobotState {
public:
    DynamicStateCreate( RobotStateLoginConnectted, RobotState_Login_Connectted );

    void OnEnterState( ) override;
};

class RobotStateLoginLogined : public RobotState {
public:
    DynamicStateCreate( RobotStateLoginLogined, RobotState_Login_Logined );
};
