#pragma once
#include "libserver/console.h"

class ConsoleCmdCreate :public ConsoleCmd
{
public:
    void RegisterHandler() override;
    void HandleHelp() override;

protected:
    void HandleShowAllWorld(std::vector<std::string>& params);

};
