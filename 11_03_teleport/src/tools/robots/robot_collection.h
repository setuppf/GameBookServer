#pragma once
#include "libserver/entity.h"
#include "libserver/system.h"
#include "robot.h"

class RobotCollection:public Entity<RobotCollection>, public IAwakeSystem<>
{
public:    
    void Awake() override;
    void BackToPool() override;

    void CreateRobot(std::string account);
    Robot* GetMsgObj(NetIdentify* pIdentify);

protected:
    void HandleNetworkConnected(Robot* pRobot, Packet* pPacket);
    void HandleNetworkDisconnect(Robot* pRobot, Packet* pPacket);

    void HandleHttpOuterResponse(Robot* pRobot, Packet* pPacket);

    void HandleAccountCheckRs(Robot* pRobot, Packet* pPacket);
    void HandlePlayerList(Robot* pRobot, Packet* pPacket);
    void HandleGameToken(Robot* pRobot, Packet* pPacket);
    void HandleLoginByTokenRs(Robot* pRobot, Packet* pPacket);
    void HandleEnterWorld(Robot* pRobot, Packet* pPacket);

private:
    std::map<std::string, Robot*> _robots;
};

