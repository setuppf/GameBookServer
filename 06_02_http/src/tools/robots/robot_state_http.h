#pragma once

#include "robot_state.h"
#include <string>

class RobotStateHttpConnectting : public RobotState
{
public:
    DynamicStateCreate(RobotStateHttpConnectting, RobotStateType::Http_Connecting);

    void OnEnterState() override;
    RobotStateType OnUpdate() override;

private:
    std::string _ip{ "" };
    int _port{ 0 };
    std::string _method{ "" };
};

class RobotStateHttpConnectted : public RobotState
{
public:
    DynamicStateCreate(RobotStateHttpConnectted, RobotStateType::Http_Connected);
    void OnEnterState() override;
    RobotStateType OnUpdate() override;
};