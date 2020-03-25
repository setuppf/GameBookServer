#include "robot.h"

#include "robot_state_login.h"
#include "robot_state_http.h"
#include "robot_state_game.h"
#include "global_robots.h"

#include "libserver/common.h"
#include "libserver/robot_state_type.h"
#include "libserver/yaml.h"
#include "libserver/entity_system.h"
#include "libserver/log4_help.h"
#include "libserver/message_system_help.h"
#include "libserver/message_component.h"
#include "libserver/update_component.h"

#include <json/reader.h>
#include "robot_component_gametoken.h"
#include "robot_component_login.h"

void Robot::Awake(std::string account)
{
    Player::Awake(nullptr, account);

    // update
    auto pUpdateComponent = AddComponent<UpdateComponent>();
    pUpdateComponent->UpdataFunction = BindFunP0(this, &Robot::Update);

    // message    
    auto pMsgCallBack = new MessageCallBackFunctionFilterObj<Robot>();
    AddComponent<MessageComponent>(pMsgCallBack);
    pMsgCallBack->GetPacketObject = [this](NetworkIdentify* pIdentify)
    {
        if (_objKey == pIdentify->GetObjectKey())
            return this;

        return static_cast<Robot*>(nullptr);
    };

    pMsgCallBack->RegisterFunctionWithObj(Proto::MsgId::MI_NetworkConnected, BindFunP2(this, &Robot::HandleNetworkConnected));
    pMsgCallBack->RegisterFunctionWithObj(Proto::MsgId::MI_NetworkDisconnect, BindFunP2(this, &Robot::HandleNetworkDisconnect));
    pMsgCallBack->RegisterFunctionWithObj(Proto::MsgId::MI_HttpOuterResponse, BindFunP2(this, &Robot::HandleHttpOuterResponse));

    pMsgCallBack->RegisterFunctionWithObj(Proto::MsgId::C2L_AccountCheckRs, BindFunP2(this, &Robot::HandleAccountCheckRs));
    pMsgCallBack->RegisterFunctionWithObj(Proto::MsgId::L2C_PlayerList, BindFunP2(this, &Robot::HandlePlayerList));
    pMsgCallBack->RegisterFunctionWithObj(Proto::MsgId::L2C_GameToken, BindFunP2(this, &Robot::HandleGameToken));
    pMsgCallBack->RegisterFunctionWithObj(Proto::MsgId::C2G_LoginByTokenRs, BindFunP2(this, &Robot::HandleLoginByTokenRs));

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
    _socketKey.Clean();
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

}

void Robot::HandleNetworkConnected(Robot* pRobot, Packet* pPacket)
{
    _socketKey = pPacket->GetSocketKey();
}

void Robot::HandleNetworkDisconnect(Robot* pRobot, Packet* pPacket)
{
    // 有可能已经在tcp connector 之后， http 的断开才来
    if (_socketKey.NetType != pPacket->GetSocketKey().NetType)
        return;

    _socketKey = { INVALID_SOCKET, NetworkType::None };
}

void Robot::HandleHttpOuterResponse(Robot* pRobot, Packet* pPacket)
{
    auto protoHttp = pPacket->ParseToProto<Proto::Http>();
    const int code = protoHttp.status_code();
    if (code == 200)
    {
        const auto response = protoHttp.body();
        Json::CharReaderBuilder readerBuilder;
        Json::CharReader* jsonReader = readerBuilder.newCharReader();

        Json::Value value;
        JSONCPP_STRING errs;

        const bool ok = jsonReader->parse(response.data(), response.data() + response.size(), &value, &errs);
        if (ok && errs.empty())
        {
            const auto code = value["returncode"].asInt();
            if (code == Proto::LoginHttpReturnCode::LHRC_OK)
            {
                AddComponent<RobotComponentLogin>(value["ip"].asString(), value["port"].asInt());
                ChangeState(RobotStateType::Login_Connecting);
            }
        }
        else
        {
            LOG_ERROR("json parse failed. " << response.c_str());
        }

        delete jsonReader;
    }
    else
    {
        LOG_ERROR("http response error:" << code << "\r\n" << protoHttp.body().c_str());
    }
}

void Robot::HandleAccountCheckRs(Robot* pRobot, Packet* pPacket)
{
    Proto::AccountCheckRs proto = pPacket->ParseToProto<Proto::AccountCheckRs>();

    if (pRobot->GetAccount() != this->GetAccount())
    {
        LOG_ERROR("somethin error. pRobot != this");
    }

    if (GlobalRobots::GetInstance()->GetRobotsCount() == 1)
        LOG_DEBUG("account check result account:" << _account.c_str() << " code:" << proto.return_code());

    if (proto.return_code() == Proto::AccountCheckReturnCode::ARC_OK)
    {
        ChangeState(RobotStateType::Login_Logined);
    }
    else
    {
        const google::protobuf::EnumDescriptor* descriptor = Proto::AccountCheckReturnCode_descriptor();
        const auto name = descriptor->FindValueByNumber(proto.return_code())->name();
        std::cout << "account check failed. account:" << _account << " code:" << name.c_str() << std::endl;
    }
}

void Robot::HandlePlayerList(Robot* pRobot, Packet* pPacket)
{
    auto protoList = pPacket->ParseToProto <Proto::PlayerList>();

    const auto pRandomEngine = GetSystemManager()->GetRandomEngine();

    if (protoList.player_size() == 0)
    {
        // create
        Proto::CreatePlayer protoCreate;

        // 账号名为角色名
        protoCreate.set_name(_account.c_str());

        // 随机性别
        std::uniform_int_distribution<int> disGender(0, 1);
        const int nGender = disGender(*pRandomEngine);
        if (nGender == 1)
            protoCreate.set_gender(Proto::Gender::male);
        else
            protoCreate.set_gender(Proto::Gender::female);

        //LOG_DEBUG("create. name:" << playerName.c_str() << " gender:" << (int)Proto::Gender::male);
        MessageSystemHelp::SendPacket(Proto::MsgId::C2L_CreatePlayer, pPacket, protoCreate);
    }
    else
    {
        std::uniform_int_distribution<int> disIndex(0, protoList.player_size() - 1);
        const int index = disIndex(*pRandomEngine);

        auto protoPlayer = protoList.player(index);

        Proto::SelectPlayer protoSelect;
        protoSelect.set_player_sn(protoPlayer.sn());
        _playerSn = protoPlayer.sn();

        MessageSystemHelp::SendPacket(Proto::MsgId::C2L_SelectPlayer, pPacket, protoSelect);

        ChangeState(RobotStateType::Login_SelectPlayer);
    }
}

void Robot::HandleGameToken(Robot* pRobot, Packet* pPacket)
{
    auto proto = pPacket->ParseToProto<Proto::GameToken>();
    LOG_DEBUG("HandleGameToken. token:" << proto.token().c_str() << " ip:" << proto.ip().c_str() << " port:" << proto.port());

    if (proto.return_code() != Proto::GameToken_ReturnCode_GameToken_OK)
    {
        LOG_DEBUG("HandleLoginGame. not found game process.");
        return;
    }

    AddComponent<RobotComponentGameToken>(proto.token().c_str(), proto.ip().c_str(), proto.port());
    ChangeState(RobotStateType::Game_Connecting);
}

void Robot::HandleLoginByTokenRs(Robot* pRobot, Packet* pPacket)
{
    auto protoRs = pPacket->ParseToProto<Proto::LoginByTokenRs>();
    if (protoRs.return_code() != Proto::LoginByTokenRs::LGRC_OK)
    {
        LOG_ERROR("login game failed. return_code:" << protoRs.return_code());
        return;
    }

    ChangeState(RobotStateType::Game_Logined);
}
