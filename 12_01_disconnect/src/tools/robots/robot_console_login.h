#pragma once
#include "libserver/console.h"
#include <list>

class RobotConsoleLogin :public ConsoleCmd
{
public:
	void RegisterHandler() override;
	void HandleHelp() override;

private:
	void HandleLogin(std::vector<std::string>& params);
	void HandleLoginEx(std::vector<std::string>& params) const;
};

