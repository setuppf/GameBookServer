#pragma once

#include "libserver/util_time.h"
#include "libserver/robot_state_type.h"
#include "libserver/network_connector.h"

class RobotMgr : public NetworkConnector, public IAwakeFromPoolSystem<>
{
public:
    void AwakeFromPool() override;
    void RegisterMsgFunction() override;    
    void Update() override;

    void ShowInfo();
private:
    void HandleRobotState(Packet* pPacket);
    void NofityServer(RobotStateType maxType);

private:
    bool _isChange{ false };
    // <account, RobotStateType>
    std::map<std::string, RobotStateType> _robots;

    timeutil::Time _nextShowInfoTime{ 0 };
};

