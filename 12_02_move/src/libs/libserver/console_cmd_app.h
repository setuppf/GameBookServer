#pragma once
#include "console.h"

class ConsoleCmdApp :public ConsoleCmd
{
public:
    void RegisterHandler() override;
    void HandleHelp() override;

private:
    void HandleAppInfo(std::vector<std::string>& params);

};

