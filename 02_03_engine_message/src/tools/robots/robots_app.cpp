#include "robots_app.h"
#include "robot.h"

void RobotsApp::InitApp()
{
	for (int i = 0; i < 10; i++)
	{
		Robot* pRobot = new Robot();
		_pThreadMgr->AddObjToThread(pRobot);
	}
}

