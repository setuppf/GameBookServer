#include "console_cmd_pool.h"
#include "object_pool.h"
#include "connect_obj.h"
#include "packet.h"

void ConsoleCmdPool::RegisterHandler()
{
	OnRegisterHandler("-show", BindFunP1(this, &ConsoleCmdPool::HandleShow));
}

void ConsoleCmdPool::HandleHelp()
{
	std::cout << "\t-show name.\tshow information of 'name' pool. Values are as follows:\n\t\tpacket\n\t\tconnect" << std::endl;
}

void ConsoleCmdPool::HandleShow(std::vector<std::string>& params)
{
	if (!CheckParamCnt(params, 1))
		return;

	const std::string poolName = params[0];
	if (poolName == "packet")
	{
		//PacketPool::GetInstance()->Show();
	}
	else if (poolName == "connect")
	{
		DynamicObjectPool<ConnectObj>::GetInstance()->Show();
	}
}
