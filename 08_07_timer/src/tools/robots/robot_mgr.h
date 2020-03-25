#pragma once

#include "libserver/util_time.h"
#include "libserver/robot_state_type.h"
#include "libserver/network_connector.h"

class RobotMgr : public NetworkConnector, public IAwakeFromPoolSystem<>
{
public:
    void Awake() override;
    void ShowInfo();

    static bool IsSingle() { return true; }

private:
    void HandleRobotState(Packet* pPacket);
    void NofityServer(RobotStateType maxType);

private:
    bool _isChange{ false };
    // <account, RobotStateType>
    std::map<std::string, RobotStateType> _robots;
};

