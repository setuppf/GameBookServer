#include "console_cmd_world_proxy.h"
#include "libserver/message_system_help.h"

void ConsoleCmdWorldProxy::RegisterHandler()
{
	OnRegisterHandler("-all", BindFunP1(this, &ConsoleCmdWorldProxy::HandleShowAllWorld));
}

void ConsoleCmdWorldProxy::HandleHelp()
{
	std::cout << "\t-all.\t\tshow all proxyworlds" << std::endl;
}

void ConsoleCmdWorldProxy::HandleShowAllWorld(std::vector<std::string>& params)
{
	MessageSystemHelp::DispatchPacket(Proto::MsgId::MI_CmdWorldProxy, nullptr);
}
