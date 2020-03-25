#include "robot.h"

bool Robot::Init()
{
	if (!NetworkConnector::Init())
		return false;

	this->Connect("127.0.0.1", 2233);
	return true;
}

void Robot::RegisterMsgFunction()
{
	NetworkConnector::RegisterMsgFunction();
}

void Robot::Update()
{
	NetworkConnector::Update();
}
