#pragma once
#include "console.h"

class ConsoleCmdThread :public ConsoleCmd
{
public:
	void RegisterHandler() override;
	void HandleHelp() override;

private:
	void HandleEntity(std::vector<std::string>& params);
    void HandlePool(std::vector<std::string>& params);
    void HandleConnect(std::vector<std::string>& params);
};
