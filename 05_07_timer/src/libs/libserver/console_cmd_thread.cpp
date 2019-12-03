#include "console_cmd_thread.h"
#include "message_system_help.h"
#include "common.h"
#include "object_pool_packet.h"

void ConsoleCmdThread::RegisterHandler()
{
	OnRegisterHandler("-entity", BindFunP1(this, &ConsoleCmdThread::HandleEntity));
    OnRegisterHandler("-pool", BindFunP1(this, &ConsoleCmdThread::HandlePool));
}

void ConsoleCmdThread::HandleHelp()
{
	std::cout << "\t-entity.\t\tshow entities in thread." << std::endl;
    std::cout << "\t-pool.  \t\tshow the information of pools in thread." << std::endl;
}

void ConsoleCmdThread::HandleEntity(std::vector<std::string>& params)
{
    Proto::CmdThread cmdProto;
    cmdProto.set_cmd_type(Proto::CmdThread_CmdType_Entity);
	MessageSystemHelp::DispatchPacket(Proto::MsgId::MI_CmdThread, 0, cmdProto);
}

void ConsoleCmdThread::HandlePool(std::vector<std::string>& params)
{
    LOG_DEBUG("------------------------------------");
    DynamicPacketPool::GetInstance()->Show();

    Proto::CmdThread cmdProto;
    cmdProto.set_cmd_type(Proto::CmdThread_CmdType_Pool);
    MessageSystemHelp::DispatchPacket(Proto::MsgId::MI_CmdThread, 0, cmdProto);
}
