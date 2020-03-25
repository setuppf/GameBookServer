#include "robot.h"

#include "robot_state_login.h"
#include "robot_state_http.h"
#include "robot_state_game.h"
#include "global_robots.h"

#include "libserver/common.h"
#include "libserver/robot_state_type.h"
#include "libserver/entity_system.h"
#include "libserver/log4_help.h"
#include "libserver/message_system_help.h"
#include "libserver/update_component.h"

#include <json/reader.h>

#include "libserver/message_system.h"

void Robot::Awake(std::string account)
{
    Player::Awake(nullptr, account);

    // update
    AddComponent<UpdateComponent>(BindFunP0(this, &Robot::Update));

    InitStateTemplateMgr(RobotStateType::Http_Connecting);
}

void Robot::BackToPool()
{
    _account = "";
    Player::BackToPool();
}

void Robot::Update()
{
    UpdateState();
}

void Robot::NetworkDisconnect()
{
    if (_socketKey.Socket == INVALID_SOCKET)
        return;

    MessageSystemHelp::DispatchPacket(Proto::MsgId::MI_NetworkRequestDisconnect, this);
    _socketKey.Clear();
}

void Robot::RegisterState()
{
    RegisterStateClass(RobotStateType::Http_Connecting, DynamicStateBind(RobotStateHttpConnecting));
    RegisterStateClass(RobotStateType::Http_Connected, DynamicStateBind(RobotStateHttpConnected));

    RegisterStateClass(RobotStateType::Login_Connecting, DynamicStateBind(RobotStateLoginConnecting));
    RegisterStateClass(RobotStateType::Login_Connected, DynamicStateBind(RobotStateLoginConnected));
    RegisterStateClass(RobotStateType::Login_Logined, DynamicStateBind(RobotStateLoginLogined));
    RegisterStateClass(RobotStateType::Login_SelectPlayer, DynamicStateBind(RobotStateLoginSelectPlayer));

    RegisterStateClass(RobotStateType::Game_Connecting, DynamicStateBind(RobotStateGameConnecting));
    RegisterStateClass(RobotStateType::Game_Connected, DynamicStateBind(RobotStateGameConnected));
    RegisterStateClass(RobotStateType::Game_Logined, DynamicStateBind(RobotStateGameLogined));

    RegisterStateClass(RobotStateType::Space_EnterWorld, DynamicStateBind(RobotStateSpaceEnterWorld));
}

void Robot::EnterWorld(int worldId)
{
    _worldId = worldId;
    if (_pState->GetState() == RobotStateType::Space_EnterWorld)
        return;

    ChangeState(RobotStateType::Space_EnterWorld);
}