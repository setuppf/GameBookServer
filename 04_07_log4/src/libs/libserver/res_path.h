#pragma once
#include "singleton.h"

class ResPath : public Singleton<ResPath>
{
public:
    ResPath();

	std::string FindResPath(const std::string& res);
	std::string FindResPath(const char* res);
	
private:
	std::string _resPath;
};


