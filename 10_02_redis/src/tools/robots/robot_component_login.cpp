#include "robot_component_login.h"

void RobotComponentLogin::Awake(std::string ip, int port)
{
    _loginIp = ip;
    _loginPort = port;
}

void RobotComponentLogin::BackToPool()
{

}
