#pragma once

#include "libserver/network_connector.h"
#include "libserver/state_template.h"
#include "libserver/robot_state_type.h"

#include "robot_state.h"

class Robot : public NetworkConnector, public StateTemplateMgr<RobotStateType, RobotState, Robot>, public IAwakeFromPoolSystem<std::string>
{
public:
	void AwakeFromPool(std::string account) override;
	void RegisterMsgFunction( ) override;
	void Update( ) override;

	std::string GetAccount() const;
	void SendMsgAccountCheck();

protected:
	void RegisterState() override;

private:
	void HandleAccountCheckRs(Robot* pRobot, Packet* pPacket);
    void HandlePlayerList(Robot* pRobot, Packet* pPacket);

private:
	std::string _account;

};

