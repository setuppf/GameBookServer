#include "console_cmd_thread.h"
#include "message_system_help.h"

void ConsoleCmdThread::RegisterHandler()
{
	OnRegisterHandler("-entity", BindFunP1(this, &ConsoleCmdThread::HandleEntity));
}

void ConsoleCmdThread::HandleHelp()
{
	std::cout << "\t-entity.\t\tshow all thread's entity." << std::endl;
}

void ConsoleCmdThread::HandleEntity(std::vector<std::string>& params)
{
	MessageSystemHelp::DispatchPacket(Proto::MsgId::MI_CmdShowThreadEntites, 0);
}
