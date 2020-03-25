#pragma once
#include "libserver/component.h"
#include "libserver/system.h"

class RobotComponentCreate:public Component<RobotComponentCreate>, public IAwakeSystem<std::string, int, int>
{
public:
    void Awake(std::string account, int min, int max) override;
    void BackToPool() override;
    
};

