#pragma once

#include "robot_state.h"
#include "libserver/common.h"

class RobotStateLoginConnecting : public RobotState {
public:
    DynamicStateCreate(RobotStateLoginConnecting, RobotStateType::Login_Connecting);

    void OnEnterState() override;
    RobotStateType OnUpdate() override;
};

class RobotStateLoginConnected : public RobotState {
public:
    DynamicStateCreate(RobotStateLoginConnected, RobotStateType::Login_Connected );

    void OnEnterState( ) override;
    RobotStateType OnUpdate() override;
};

class RobotStateLoginLogined : public RobotState {
public:
    DynamicStateCreate( RobotStateLoginLogined, RobotStateType::Login_Logined );
    RobotStateType OnUpdate() override;
};

class RobotStateLoginSelectPlayer : public RobotState
{
public:
    DynamicStateCreate(RobotStateLoginSelectPlayer, RobotStateType::Login_SelectPlayer);
};