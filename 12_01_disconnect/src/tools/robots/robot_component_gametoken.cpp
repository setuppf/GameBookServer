#include "robot_component_gametoken.h"

void RobotComponentGameToken::Awake(std::string token, std::string ip, int port)
{
    Update(token, ip, port);
}

void RobotComponentGameToken::BackToPool()
{
}

void RobotComponentGameToken::Update(std::string token, std::string ip, int port)
{
    _token = token;
    _gameIp = ip;
    _gamePort = port;
}
