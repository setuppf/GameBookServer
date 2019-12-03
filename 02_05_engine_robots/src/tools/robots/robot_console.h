#pragma once
#include "libserver/console.h"

class RobotConsole : public Console
{
public:
	bool Init() override;
};

class RobotConsoleLogin :public ConsoleCmd
{
public:
	void RegisterHandler() override;

private:
	void HandleHelp(std::string p1, std::string p2);
	void HandleLogin(std::string p1, std::string p2);
	void HandleLoginEx(std::string p1, std::string p2);	
};