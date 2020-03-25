#pragma once

#include "libserver/network_connector.h"
#include "libserver/state_template.h"
#include "libserver/robot_state_type.h"

#include "robot_state.h"

class Robot : public NetworkConnector, public StateTemplateMgr<RobotStateType, RobotState, Robot>, public IAwakeFromPoolSystem<std::string>
{
public:
	void Awake(std::string account);
	void Update() override;

	std::string GetAccount() const;
	void SendMsgAccountCheck();

    static bool IsSingle() { return false; }

protected:
	void RegisterState() override;

private:
	void HandleAccountCheckRs(Robot* pRobot, Packet* pPacket);
    void HandlePlayerList(Robot* pRobot, Packet* pPacket);

private:
	std::string _account;

};

