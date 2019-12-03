#pragma once
#include "libserver/console.h"

class ConsoleCmdWorldProxy :public ConsoleCmd
{
public:
	void RegisterHandler() override;
	void HandleHelp() override;

protected:
	void HandleShowAllWorld(std::vector<std::string>& params);
};

