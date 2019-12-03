#pragma once
#include "libserver/state_template.h"
#include "libserver/robot_state_type.h"

class Robot;

class RobotState : public StateTemplate<RobotStateType, Robot>
{
public:
    RobotStateType Update() override;
    virtual RobotStateType OnUpdate()
    {
        return GetState();
    }

    void EnterState() override;
    virtual void OnEnterState()
    {
    }

    void LeaveState() override;
    virtual void OnLeaveState()
    {
    }
};

