#pragma once
#include "libserver/component.h"
#include "libserver/system.h"

class RobotComponentLogin :public Component<RobotComponentLogin>, public IAwakeFromPoolSystem<std::string, int>
{
public:
    void Awake(std::string ip, int port) override;
    void BackToPool() override;

    std::string GetLoginIp() const { return _loginIp; }
    int GetLoginPort() const { return _loginPort; }

private:
    std::string _loginIp{ "" };
    int _loginPort{ 0 };
};

