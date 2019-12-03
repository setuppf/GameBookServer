#include "robot_console_world.h"
#include "libresource/resource_manager.h"
#include "libserver/thread_mgr.h"
#include "robot.h"
#include "libserver/message_system.h"
#include "robot_locator.h"

void RobotConsoleWorld::RegisterHandler()
{
	OnRegisterHandler("-enter", BindFunP1(this, &RobotConsoleWorld::HandleEnter));
}

void RobotConsoleWorld::HandleHelp()
{
	std::cout << "\t-enter {0}. enter world\t\t{0} = world id" << std::endl;
}

void RobotConsoleWorld::HandleEnter(std::vector<std::string>& params) const
{
	if (!CheckParamCnt(params, 1))
		return;

    auto pGlobalEntitySystem = ComponentHelp::GetGlobalEntitySystem();
	const auto worldId = std::stoi(params[0]);

	// 检查一下合法性
	auto pResMgr = pGlobalEntitySystem->GetComponent<ResourceManager>();
	const auto pWorldRes = pResMgr->Worlds->GetResource(worldId);
	if (pWorldRes == nullptr)
	{
		LOG_DEBUG("input error. can't find world'res. id:" << worldId);
		return;
	}

    NetIdentify netIdentify;
	auto pRobotLocator = pGlobalEntitySystem->GetComponent<RobotLocator>();
	if (!pRobotLocator->GetRobotNetIdentify(&netIdentify))
	{
		LOG_DEBUG("input error. can't find robot. login first.");
		return;
	}

	Proto::EnterWorld proto;
	proto.set_world_id(worldId);
	MessageSystemHelp::SendPacket(Proto::MsgId::C2G_EnterWorld, proto, &netIdentify);
}
