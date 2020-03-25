#pragma once
#include "console.h"

class ConsoleCmdTrace :public ConsoleCmd
{
public:
    void RegisterHandler() override;
    void HandleHelp() override;

private:
    void HandleConnect(std::vector<std::string>& params);
    void HandlePacket(std::vector<std::string>& params);
    void HandlePlayer(std::vector<std::string>& params);
    void HandleAccount(std::vector<std::string>& params);
    void HandleTime(std::vector<std::string>& params);

    void HandleClean(std::vector<std::string>& params);
};

