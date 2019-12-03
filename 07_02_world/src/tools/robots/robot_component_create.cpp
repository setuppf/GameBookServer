#include "robot_component_create.h"
#include "robot_collection.h"

#include "libserver/log4_help.h"
#include "libserver/system_manager.h"

#include <thread>

void RobotComponentCreate::Awake(std::string account, int min, int max)
{
    // 创建的Robot不包括Max值 [min, max)
    //LOG_DEBUG("create robot. account:" << account.c_str() << " min:" << min << " max:" << max << " thread id:" << std::this_thread::get_id());
    auto pCollection = GetSystemManager()->GetEntitySystem()->GetComponent<RobotCollection>();
    if (min == 0 && max == 0)
    {
        pCollection->CreateRobot(account);
    }
    else
    {
        for (int i = min; i < max; i++)
        {
            pCollection->CreateRobot(account + std::to_string(i));
        }
    }
}

void RobotComponentCreate::BackToPool()
{
}
