#pragma once
#include "libserver/component.h"
#include "libserver/system.h"

class RobotComponentGameToken :public Component<RobotComponentGameToken>, public IAwakeFromPoolSystem<std::string, std::string, int>
{
public:
    void Awake(std::string token, std::string ip, int port) override;
    void BackToPool() override;

    void Update(std::string token, std::string ip, int port);

    std::string GetToken() const { return _token; }
    std::string GetGameIp() const { return _gameIp; }
    int GetGamePort() const { return _gamePort; }

private:
    std::string _token{ "" };
    std::string _gameIp{ "" };
    int _gamePort{ 0 };
};

