#pragma once
#include "libserver/entity.h"
#include "libserver/system.h"
#include "robot.h"

#include <mutex>
#include <map>

class RobotCollection:public Entity<RobotCollection>, public IAwakeSystem<std::string, int, int>
{
public:    
    void Awake(std::string account, int min, int max) override;
    void BackToPool() override;

protected:
    void CreateRobot(std::string account);
    Robot* GetMsgObj(NetIdentify* pIdentify);

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

