#pragma once

#include "libserver/state_template.h"
#include "libserver/robot_state_type.h"

#include "libplayer/player.h"

#include "robot_state.h"

class Robot : public Player, public StateTemplateMgr<RobotStateType, RobotState, Robot>, public IAwakeFromPoolSystem<std::string>
{
public:
    static bool IsSingle() { return false; }

    void Awake(std::string account) override;
    void BackToPool() override;    
    void Update();
    void NetworkDisconnect();

protected:
    void RegisterState() override;

private:
    void HandleNetworkConnected(Robot* pRobot, Packet* pPacket);
    void HandleNetworkDisconnect(Robot* pRobot, Packet* pPacket);
    void HandleHttpOuterResponse(Robot* pRobot, Packet* pPacket);

    void HandleAccountCheckRs(Robot* pRobot, Packet* pPacket);
    void HandlePlayerList(Robot* pRobot, Packet* pPacket);
    void HandleGameToken(Robot* pRobot, Packet* pPacket);
    void HandleLoginByTokenRs(Robot* pRobot, Packet* pPacket);
};

