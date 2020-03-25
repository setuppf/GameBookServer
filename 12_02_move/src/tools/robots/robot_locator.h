#pragma once
#include "libserver/component.h"
#include "libserver/system.h"
#include "libserver/socket_object.h"

class Robot;

class RobotLocator :public Component<RobotLocator>, public IAwakeSystem<>
{
public:
	void Awake() override;
    void BackToPool() override;

	void RegisterToLocator(Robot* pRobot);
    bool GetRobotNetIdentify(NetIdentify* pNetIdentify);

private:
	std::mutex _lock;
	std::map<uint64, NetIdentify> _robots;
};

