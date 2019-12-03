#pragma once
#include "console.h"

class ConsoleCmdEfficiency :public ConsoleCmd
{
public:
	void RegisterHandler() override;
	void HandleHelp() override;

private:
	void HandleThread(std::vector<std::string>& params);
};

