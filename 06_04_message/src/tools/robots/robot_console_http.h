#pragma once
#include "libserver/console.h"

class RobotConsoleHttp :public ConsoleCmd
{
public:
	void RegisterHandler() override;
	void HandleHelp() override;

private:	
	void HandleRequest(std::vector<std::string>& params);
};
