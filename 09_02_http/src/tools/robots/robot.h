#pragma once

#include "libserver/network_connector.h"
#include "libserver/state_template.h"
#include "libserver/robot_state_type.h"
#include "libserver/socket_object.h"

#include "robot_state.h"

class Robot : public Entity<Robot>, public NetworkIdentify, public StateTemplateMgr<RobotStateType, RobotState, Robot>, public IAwakeFromPoolSystem<std::string>
{
public:
    void Awake(std::string account) override;
    void BackToPool() override;

    void Update();

    std::string GetAccount() const;
    void ConnectToLogin() const;

protected:
    void RegisterState() override;

private:
    void HandleNetworkConnected(Robot* pRobot, Packet* pPacket);
    void HandleNetworkDisconnect(Robot* pRobot, Packet* pPacket);
    void HandleHttpOuterResponse(Robot* pRobot, Packet* pPacket);

    void HandleAccountCheckRs(Robot* pRobot, Packet* pPacket);
    void HandlePlayerList(Robot* pRobot, Packet* pPacket);

private:
    std::string _account;
    std::string _loginIp{""};
    int _loginPort{ 0 };
};

