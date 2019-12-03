#include "global_robots.h"

size_t GlobalRobots::GetRobotsCount()
{
	std::lock_guard<std::mutex> guard(_mtx);
	return _robotsCnt;
}

void GlobalRobots::SetRobotsCount(const size_t count)
{
	std::lock_guard<std::mutex> guard(_mtx);
	_robotsCnt = count;
}

void GlobalRobots::CleanRobotsCount()
{
	std::lock_guard<std::mutex> guard(_mtx);
	_robotsCnt = 0;
}
