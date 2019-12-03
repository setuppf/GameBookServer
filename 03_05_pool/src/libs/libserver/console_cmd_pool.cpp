#include "console_cmd_pool.h"
#include "object_pool.h"
#include "connect_obj.h"

void ConsoleCmdPool::RegisterHandler()
{
	OnRegisterHandler("-help", BindFunP1(this, &ConsoleCmdPool::HandleHelp));
	OnRegisterHandler("-show", BindFunP1(this, &ConsoleCmdPool::HandleShow));
}

void ConsoleCmdPool::HandleHelp(std::vector<std::string>& params)
{
	std::cout << "-show name.\t\tshow connect pool info." << std::endl;
}

void ConsoleCmdPool::HandleShow(std::vector<std::string>& params)
{
	const std::string poolName = params[0];
    DynamicObjectPool<ConnectObj>::GetInstance()->Show();
}
