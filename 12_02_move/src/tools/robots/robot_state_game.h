#pragma once

#include "robot_state.h"

class RobotStateGameConnecting : public RobotState
{
public:
	DynamicStateCreate(RobotStateGameConnecting, RobotStateType::Game_Connecting);

    void OnEnterState() override;
	RobotStateType OnUpdate() override;
};

class RobotStateGameConnected : public RobotState
{
public:
    DynamicStateCreate(RobotStateGameConnected, RobotStateType::Game_Connected);

	void OnEnterState() override;
};

class RobotStateGameLogined : public RobotState
{
public:
	DynamicStateCreate(RobotStateGameLogined, RobotStateType::Game_Logined);
};

class RobotStateSpaceEnterWorld : public RobotState
{
public:
    DynamicStateCreate(RobotStateSpaceEnterWorld, RobotStateType::Space_EnterWorld);
};