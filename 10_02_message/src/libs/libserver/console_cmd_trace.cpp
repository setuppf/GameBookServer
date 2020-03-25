#include "console_cmd_trace.h"
#include "component_help.h"

void ConsoleCmdTrace::RegisterHandler()
{
    OnRegisterHandler("-connect", BindFunP1(this, &ConsoleCmdTrace::HandleConnect));
    OnRegisterHandler("-packet", BindFunP1(this, &ConsoleCmdTrace::HandlePacket));
    OnRegisterHandler("-player", BindFunP1(this, &ConsoleCmdTrace::HandlePlayer));
    OnRegisterHandler("-account", BindFunP1(this, &ConsoleCmdTrace::HandleAccount));
    OnRegisterHandler("-time", BindFunP1(this, &ConsoleCmdTrace::HandleTime));


    OnRegisterHandler("-clean", BindFunP1(this, &ConsoleCmdTrace::HandleClean));
}

void ConsoleCmdTrace::HandleHelp()
{
    std::cout << "\t-connect socket.  \t\tshow the information of socket." << std::endl;
    std::cout << "\t-packet socket.  \t\tall trace of packet related socket." << std::endl;
    std::cout << "\t-player socket.  \t\tall trace of player related socket." << std::endl;
    std::cout << "\t-account account.  \t\tall trace of account." << std::endl;
    std::cout << "\t-clean.  \t\tclean all data." << std::endl;
}

void ConsoleCmdTrace::HandleConnect(std::vector<std::string>& params)
{
    if (!CheckParamCnt(params, 1))
        return;

    const int socket = std::atoi(params[0].c_str());
    auto pTraceComponent = ComponentHelp::GetTraceComponent();
    pTraceComponent->Show(TraceType::Connector, socket);
}

void ConsoleCmdTrace::HandlePacket(std::vector<std::string>& params)
{
    if (!CheckParamCnt(params, 1))
        return;

    const int socket = std::atoi(params[0].c_str());
    auto pTraceComponent = ComponentHelp::GetTraceComponent();
    pTraceComponent->Show(TraceType::Packet, socket);
}

void ConsoleCmdTrace::HandlePlayer(std::vector<std::string>& params)
{
    if (!CheckParamCnt(params, 1))
        return;

    const int socket = std::atoi(params[0].c_str());
    auto pTraceComponent = ComponentHelp::GetTraceComponent();
    pTraceComponent->Show(TraceType::Player, socket);
}

void ConsoleCmdTrace::HandleAccount(std::vector<std::string>& params)
{
    if (!CheckParamCnt(params, 1))
        return;

    auto account = params[0];
    auto pTraceComponent = ComponentHelp::GetTraceComponent();
    pTraceComponent->ShowAccount(account);
}

void ConsoleCmdTrace::HandleTime(std::vector<std::string>& params)
{
    auto pTraceComponent = ComponentHelp::GetTraceComponent();
    pTraceComponent->Show(TraceType::Time, 0);
}

void ConsoleCmdTrace::HandleClean(std::vector<std::string>& params)
{
    auto pTraceComponent = ComponentHelp::GetTraceComponent();
    pTraceComponent->Clean();
}

