#include "robot_collection.h"
#include "libserver/message_system.h"

#include <json/reader.h>
#include "robot_component_login.h"
#include "global_robots.h"
#include "robot_component_gametoken.h"
#include "libserver/message_system_help.h"

void RobotCollection::Awake()
{
    // message    
    auto pMsgSystem = GetSystemManager()->GetMessageSystem();

    pMsgSystem->RegisterFunctionFilter<Robot>(this, Proto::MsgId::MI_NetworkConnected, BindFunP1(this, &RobotCollection::GetMsgObj), BindFunP2(this, &RobotCollection::HandleNetworkConnected));
    pMsgSystem->RegisterFunctionFilter<Robot>(this, Proto::MsgId::MI_NetworkDisconnect, BindFunP1(this, &RobotCollection::GetMsgObj), BindFunP2(this, &RobotCollection::HandleNetworkDisconnect));
    pMsgSystem->RegisterFunctionFilter<Robot>(this, Proto::MsgId::MI_HttpOuterResponse, BindFunP1(this, &RobotCollection::GetMsgObj), BindFunP2(this, &RobotCollection::HandleHttpOuterResponse));

    pMsgSystem->RegisterFunctionFilter<Robot>(this, Proto::MsgId::C2L_AccountCheckRs, BindFunP1(this, &RobotCollection::GetMsgObj), BindFunP2(this, &RobotCollection::HandleAccountCheckRs));
    pMsgSystem->RegisterFunctionFilter<Robot>(this, Proto::MsgId::L2C_PlayerList, BindFunP1(this, &RobotCollection::GetMsgObj), BindFunP2(this, &RobotCollection::HandlePlayerList));
    pMsgSystem->RegisterFunctionFilter<Robot>(this, Proto::MsgId::L2C_GameToken, BindFunP1(this, &RobotCollection::GetMsgObj), BindFunP2(this, &RobotCollection::HandleGameToken));
    pMsgSystem->RegisterFunctionFilter<Robot>(this, Proto::MsgId::C2G_LoginByTokenRs, BindFunP1(this, &RobotCollection::GetMsgObj), BindFunP2(this, &RobotCollection::HandleLoginByTokenRs));

    pMsgSystem->RegisterFunctionFilter<Robot>(this, Proto::MsgId::S2C_EnterWorld, BindFunP1(this, &RobotCollection::GetMsgObj), BindFunP2(this, &RobotCollection::HandleEnterWorld));

}

void RobotCollection::BackToPool()
{
    auto pEntitySystem = GetSystemManager()->GetEntitySystem();
    for (auto iter = _robots.begin(); iter != _robots.end(); ++iter)
    {
        pEntitySystem->RemoveComponent(iter->second);
    }

    _robots.clear();
}

void RobotCollection::CreateRobot(std::string account)
{
    auto pRobot = GetSystemManager()->GetEntitySystem()->AddComponent<Robot>(account);
    _robots.insert(std::make_pair(account, pRobot));
}

Robot* RobotCollection::GetMsgObj(NetIdentify* pIdentify)
{
    auto pTagValue = pIdentify->GetTagKey()->GetTagValue(TagType::Account);
    if (pTagValue == nullptr)
        return nullptr;

    const auto account = pTagValue->KeyStr;
    auto iter = _robots.find(account);
    if (iter == _robots.end())
        return nullptr;

    return iter->second;
}

void RobotCollection::HandleNetworkConnected(Robot* pRobot, Packet* pPacket)
{
    pRobot->GetSocketKey()->CopyFrom(pPacket->GetSocketKey());
}

void RobotCollection::HandleNetworkDisconnect(Robot* pRobot, Packet* pPacket)
{
    // 有可能已经在tcp connector 之后， http 的断开才来
    if (pRobot->GetSocketKey()->NetType != pPacket->GetSocketKey()->NetType)
        return;

    pRobot->GetSocketKey()->Clear();
}

void RobotCollection::HandleHttpOuterResponse(Robot* pRobot, Packet* pPacket)
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
                pRobot->AddComponent<RobotComponentLogin>(value["ip"].asString(), value["port"].asInt());
                pRobot->ChangeState(RobotStateType::Login_Connecting);
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

void RobotCollection::HandleAccountCheckRs(Robot* pRobot, Packet* pPacket)
{
    Proto::AccountCheckRs proto = pPacket->ParseToProto<Proto::AccountCheckRs>();
    if (GlobalRobots::GetInstance()->GetRobotsCount() == 1)
        LOG_DEBUG("account check result account:" << pRobot->GetAccount().c_str() << " code:" << proto.return_code());

    if (proto.return_code() == Proto::AccountCheckReturnCode::ARC_OK)
    {
        pRobot->ChangeState(RobotStateType::Login_Logined);
    }
    else
    {
        const google::protobuf::EnumDescriptor* descriptor = Proto::AccountCheckReturnCode_descriptor();
        const auto name = descriptor->FindValueByNumber(proto.return_code())->name();
        LOG_WARN("account check failed. account:" << pRobot->GetAccount().c_str() << " code:" << name.c_str());
    }
}

void RobotCollection::HandlePlayerList(Robot* pRobot, Packet* pPacket)
{
    auto protoList = pPacket->ParseToProto <Proto::PlayerList>();

    const auto pRandomEngine = GetSystemManager()->GetRandomEngine();

    if (protoList.player_size() == 0)
    {
        // create
        Proto::CreatePlayer protoCreate;

        // 账号名为角色名
        protoCreate.set_name(pRobot->GetAccount().c_str());

        // 随机性别
        std::uniform_int_distribution<int> disGender(0, 1);
        const int nGender = disGender(*pRandomEngine);
        if (nGender == 1)
            protoCreate.set_gender(Proto::Gender::male);
        else
            protoCreate.set_gender(Proto::Gender::female);

        //LOG_DEBUG("create. name:" << playerName.c_str() << " gender:" << (int)Proto::Gender::male);
        MessageSystemHelp::SendPacket(Proto::MsgId::C2L_CreatePlayer, protoCreate, pPacket);
    }
    else
    {
        std::uniform_int_distribution<int> disIndex(0, protoList.player_size() - 1);
        const int index = disIndex(*pRandomEngine);

        auto protoPlayer = protoList.player(index);

        Proto::SelectPlayer protoSelect;
        protoSelect.set_player_sn(protoPlayer.sn());
        MessageSystemHelp::SendPacket(Proto::MsgId::C2L_SelectPlayer, protoSelect, pPacket);

        pRobot->ChangeState(RobotStateType::Login_SelectPlayer);
    }
}

void RobotCollection::HandleGameToken(Robot* pRobot, Packet* pPacket)
{
    auto proto = pPacket->ParseToProto<Proto::GameToken>();
    //LOG_DEBUG("HandleGameToken. token:" << proto.token().c_str() << " ip:" << proto.ip().c_str() << " port:" << proto.port());

    if (proto.return_code() != Proto::GameToken_ReturnCode_GameToken_OK)
    {
        LOG_DEBUG("HandleLoginGame. not found game process.");
        return;
    }

    pRobot->AddComponent<RobotComponentGameToken>(proto.token().c_str(), proto.ip().c_str(), proto.port());
    pRobot->ChangeState(RobotStateType::Game_Connecting);
}

void RobotCollection::HandleLoginByTokenRs(Robot* pRobot, Packet* pPacket)
{
    auto protoRs = pPacket->ParseToProto<Proto::LoginByTokenRs>();
    if (protoRs.return_code() != Proto::LoginByTokenRs::LGRC_OK)
    {
        LOG_ERROR("login game failed. return_code:" << protoRs.return_code());
        return;
    }

    pRobot->ChangeState(RobotStateType::Game_Logined);
}

void RobotCollection::HandleEnterWorld(Robot* pRobot, Packet* pPacket)
{
    auto proto = pPacket->ParseToProto<Proto::EnterWorld>();
    LOG_DEBUG("account:" << pRobot->GetAccount().c_str() << " enter world. id:" << proto.world_id());

    pRobot->ChangeState(RobotStateType::Space_EnterWorld);
}
