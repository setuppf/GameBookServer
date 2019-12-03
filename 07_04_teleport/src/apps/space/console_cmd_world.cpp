#include "console_cmd_world.h"
#include "libserver/message_system_help.h"

void ConsoleCmdWorld::RegisterHandler()
{
	OnRegisterHandler("-all", BindFunP1(this, &ConsoleCmdWorld::HandleShowAllWorld));
}

void ConsoleCmdWorld::HandleHelp()
{
	std::cout << "\t-all.\t\tshow all worlds" << std::endl;

}

void ConsoleCmdWorld::HandleShowAllWorld(std::vector<std::string>& params)
{
	MessageSystemHelp::DispatchPacket(Proto::MsgId::MI_CmdWorld, nullptr);
}
