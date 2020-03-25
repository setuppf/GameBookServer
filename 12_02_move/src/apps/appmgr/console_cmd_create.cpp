#include "console_cmd_create.h"
#include "libserver/message_system_help.h"

void ConsoleCmdCreate::RegisterHandler()
{
    OnRegisterHandler("-all", BindFunP1(this, &ConsoleCmdCreate::HandleShowAllWorld));
}

void ConsoleCmdCreate::HandleHelp()
{
    std::cout << "\t-all.\t\tshow all worlds" << std::endl;
}

void ConsoleCmdCreate::HandleShowAllWorld(std::vector<std::string>& params)
{
    MessageSystemHelp::DispatchPacket(Proto::MsgId::MI_CmdCreate, nullptr);
}
