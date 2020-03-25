#include "robot_locator.h"
#include "libserver/network_connector.h"
#include "robot.h"

void RobotLocator::Awake()
{
    std::lock_guard<std::mutex> guard(_lock);
    _robots.clear();
}

void RobotLocator::RegisterToLocator(Robot* pRobot)
{
    std::lock_guard<std::mutex> guard(_lock);
    NetIdentify netIdentify;
    netIdentify.GetSocketKey()->CopyFrom(pRobot->GetSocketKey());
    netIdentify.GetTagKey()->CopyFrom(pRobot->GetTagKey());
    _robots[pRobot->GetSN()] = netIdentify;
}

bool RobotLocator::GetRobotNetIdentify(NetIdentify* pNetIdentify)
{
    std::lock_guard<std::mutex> guard(_lock);
    const auto iter = _robots.begin();
    if (iter == _robots.end())
        return false;

    auto netIdentify = iter->second;
    pNetIdentify->GetSocketKey()->CopyFrom(netIdentify.GetSocketKey());
    pNetIdentify->GetTagKey()->CopyFrom(netIdentify.GetTagKey());
    return true;
}

void RobotLocator::BackToPool()
{
    std::lock_guard<std::mutex> guard(_lock);
    _robots.clear();
}
