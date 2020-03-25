#include "robot_state_game.h"
#include "robot.h"
#include "libserver/message_system_help.h"
#include "robot_component_gametoken.h"

void RobotStateGameConnecting::OnEnterState()
{
    _pParentObj->NetworkDisconnect();

    // 请求一个新的Tcp连接
    auto tokenObj = _pParentObj->GetComponent<RobotComponentGameToken>();
    TagValue tagValue{ _pParentObj->GetAccount(), 0 };
    MessageSystemHelp::CreateConnect(NetworkType::TcpConnector, TagType::Account, tagValue, tokenObj->GetGameIp(), tokenObj->GetGamePort());
}

RobotStateType RobotStateGameConnecting::OnUpdate()
{
    auto socketKey = _pParentObj->GetSocketKey();
    if (socketKey->Socket != INVALID_SOCKET && socketKey->NetType == NetworkType::TcpConnector)
    {
        return RobotStateType::Game_Connected;
    }

    return GetState();
}

void RobotStateGameConnected::OnEnterState()
{
    auto tokenObj = _pParentObj->GetComponent<RobotComponentGameToken>();

    Proto::LoginByToken protoLogin;
    protoLogin.set_account(_pParentObj->GetAccount().c_str());
    protoLogin.set_token(tokenObj->GetToken().c_str());

    MessageSystemHelp::SendPacket(Proto::MsgId::C2G_LoginByToken, protoLogin, _pParentObj);
}

