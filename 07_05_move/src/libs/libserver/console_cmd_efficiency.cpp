#include "console_cmd_efficiency.h"
#include "message_system_help.h"

void ConsoleCmdEfficiency::RegisterHandler()
{
	OnRegisterHandler("-thread", BindFunP1(this, &ConsoleCmdEfficiency::HandleThread));
}

void ConsoleCmdEfficiency::HandleHelp()
{
	std::cout << "\t-thread.\n\t\tthread info" << std::endl;
}

void ConsoleCmdEfficiency::HandleThread(std::vector<std::string>& params)
{
	MessageSystemHelp::DispatchPacket(Proto::MsgId::MI_CmdEfficiency, 0);
}
