#pragma once

#include "libserver/state_template.h"
#include "libserver/robot_state_type.h"

#include "libplayer/player.h"

#include "robot_state.h"

class Robot : public Player, public StateTemplateMgr<RobotStateType, RobotState, Robot>, virtual public IAwakeFromPoolSystem<std::string>
{
public:
    void Awake(std::string account) override;
    void BackToPool() override;
    void Update();
    void NetworkDisconnect();
    void EnterWorld(int worldId);

protected:
    void RegisterState() override;

private:
    int _worldId{ 0 };
};

