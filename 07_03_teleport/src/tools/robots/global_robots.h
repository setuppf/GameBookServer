#pragma once
#include "libserver/singleton.h"
#include <mutex>

class GlobalRobots : public Singleton<GlobalRobots>
{
public:
	size_t GetRobotsCount();
	void SetRobotsCount(size_t count);
	void CleanRobotsCount();

private:
	std::mutex _mtx;
	size_t _robotsCnt{ 0 };
};

