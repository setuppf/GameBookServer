#pragma once

#include "robot_state.h"

class RobotStateLoginConnectting : public RobotState {
public:
    DynamicStateCreate(RobotStateLoginConnectting, RobotState_Login_Connecting);

    void OnEnterState() override;
    RobotStateType OnUpdate() override;
};

class RobotStateLoginConnectted : public RobotState {
public:
    DynamicStateCreate( RobotStateLoginConnectted, RobotState_Login_Connected );

    void OnEnterState( ) override;
};

class RobotStateLoginLogined : public RobotState {
public:
    DynamicStateCreate( RobotStateLoginLogined, RobotState_Login_Logined );
};

class RobotStateLoginSelectPlayer : public RobotState
{
public:
    DynamicStateCreate(RobotStateLoginSelectPlayer, RobotState_Login_SelectPlayer);
};