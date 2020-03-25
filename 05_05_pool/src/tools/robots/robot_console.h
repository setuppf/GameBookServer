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
    void HandleHelp(std::vector<std::string>& params);
    void HandleLogin(std::vector<std::string>& params);
    void HandleLoginEx(std::vector<std::string>& params);
};