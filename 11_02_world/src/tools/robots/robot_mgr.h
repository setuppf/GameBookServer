#pragma once

#include "libserver/robot_state_type.h"
#include "libserver/entity.h"

class RobotMgr : public Entity<RobotMgr>, public IAwakeSystem<>
{
public:
    void Awake() override;
    void BackToPool() override;

    void ShowInfo();

private:
    void HandleRobotState(Packet* pPacket);
    void NotifyServer(RobotStateType iType);

private:
    std::chrono::system_clock::time_point _start;
    bool _isChange{ false };

    // <account, RobotStateType>
    std::map<std::string, RobotStateType> _robots;
};

