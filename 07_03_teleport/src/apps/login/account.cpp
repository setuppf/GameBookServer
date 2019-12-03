#include "account.h"
#include "libserver/common.h"
#include "libserver/thread_mgr.h"
#include "libserver/log4_help.h"
#include "libserver/message_system_help.h"
#include "libserver/component_help.h"

#include "libserver/global.h"
#include "libplayer/player_collector_component.h"
#include "player_component_account.h"
#include "libplayer/player_component_proto_list.h"
#include "libserver/message_system.h"
#include "libplayer/player.h"
#include "player_component_onlineinlogin.h"
#include "libresource/resource_help.h"

void Account::Awake()
{
    AddComponent<PlayerCollectorComponent>();

    // http
    auto pYaml = ComponentHelp::GetYaml();
    const auto pLoginConfig = dynamic_cast<LoginConfig*>(pYaml->GetConfig(APP_LOGIN));
    ParseUrlInfo info;
    if (!MessageSystemHelp::ParseUrl(pLoginConfig->UrlLogin, info))
    {
        LOG_ERROR("parse login url failed. url:" << pLoginConfig->UrlLogin.c_str());
    }
    else
    {
        _httpIp = info.Host;
        _httpPort = info.Port;
        _method = info.Mothed;
    }

    // timer
    AddTimer(0, 10, true, 2, BindFunP0(this, &Account::SyncAppInfoToAppMgr));

    // message
    auto pMsgSystem = GetSystemManager()->GetMessageSystem();

    // check account
    pMsgSystem->RegisterFunction(this, Proto::MsgId::C2L_AccountCheck, BindFunP1(this, &Account::HandleAccountCheck));
    pMsgSystem->RegisterFunction(this, Proto::MsgId::MI_AccountQueryOnlineToRedisRs, BindFunP1(this, &Account::HandleAccountQueryOnlineToRedisRs));
    pMsgSystem->RegisterFunction(this, Proto::MsgId::MI_HttpOuterResponse, BindFunP1(this, &Account::HandleHttpOuterResponse));

    // db
    pMsgSystem->RegisterFunction(this, Proto::MsgId::L2DB_QueryPlayerListRs, BindFunP1(this, &Account::HandleQueryPlayerListRs));

    // 处理断线
    pMsgSystem->RegisterFunction(this, Proto::MsgId::MI_NetworkDisconnect, BindFunP1(this, &Account::HandleNetworkDisconnect));
    pMsgSystem->RegisterFunction(this, Proto::MsgId::MI_NetworkConnected, BindFunP1(this, &Account::HandleNetworkConnected));

    // sync
    pMsgSystem->RegisterFunction(this, Proto::MsgId::MI_AppInfoListSync, BindFunP1(this, &Account::HandleAppInfoListSync));

    // player
    pMsgSystem->RegisterFunction(this, Proto::MsgId::C2L_CreatePlayer, BindFunP1(this, &Account::HandleCreatePlayer));
    pMsgSystem->RegisterFunction(this, Proto::MsgId::L2DB_CreatePlayerRs, BindFunP1(this, &Account::HandleCreatePlayerRs));
    pMsgSystem->RegisterFunction(this, Proto::MsgId::C2L_SelectPlayer, BindFunP1(this, &Account::HandleSelectPlayer));

    // token
    pMsgSystem->RegisterFunction(this, Proto::MsgId::MI_LoginTokenToRedisRs, BindFunP1(this, &Account::HandleTokenToRedisRs));
}

void Account::BackToPool()
{
    _apps.clear();
}

void Account::SyncAppInfoToAppMgr()
{
    Proto::AppInfoSync protoSync;
    protoSync.set_app_id(Global::GetInstance()->GetCurAppId());
    protoSync.set_app_type((int)Global::GetInstance()->GetCurAppType());
    protoSync.set_online(GetComponent<PlayerCollectorComponent>()->OnlineSize());

    MessageSystemHelp::SendPacket(Proto::MsgId::MI_AppInfoSync, protoSync, APP_APPMGR);
}

void Account::HandleAppInfoListSync(Packet* pPacket)
{
    auto proto = pPacket->ParseToProto<Proto::AppInfoListSync>();
    for (auto one : proto.apps())
    {
        Parse(one, INVALID_SOCKET);
    }
}

void Account::HandleNetworkConnected(Packet* pPacket)
{
    auto pTagValue = pPacket->GetTagKey()->GetTagValue(TagType::Account);
    if (pTagValue == nullptr)
        return;

    if (pPacket->GetSocketKey()->NetType != NetworkType::HttpConnector)
        return;

    auto account = pTagValue->KeyStr;
    auto pPlayerCollector = GetComponent<PlayerCollectorComponent>();

    // http 已经连接上了
    auto pPlayer = pPlayerCollector->GetPlayerByAccount(account);
    if (pPlayer == nullptr)
    {
        LOG_ERROR("http connected. but can't find player. account:" << account.c_str() << pPacket);
        return;
    }

    const auto pPlayerCAccount = pPlayer->GetComponent<PlayerComponentAccount>();

#ifdef LOG_TRACE_COMPONENT_OPEN
    ComponentHelp::GetTraceComponent()->TraceAccount(pPlayer->GetAccount(), pPacket->GetSocketKey()->Socket);
#endif

    // 发送验证需求
    NetIdentify httpIndentify;
    httpIndentify.GetSocketKey()->CopyFrom(pPacket->GetSocketKey());
    httpIndentify.GetTagKey()->CopyFrom(pPlayer->GetTagKey());

    std::map<std::string, std::string> params;
    params["account"] = pPlayer->GetAccount();
    params["password"] = pPlayerCAccount->GetPassword();
    MessageSystemHelp::SendHttpRequest(&httpIndentify, _httpIp, _httpPort, _method, &params);
}

void Account::HandleNetworkDisconnect(Packet* pPacket)
{
    const auto socketKey = pPacket->GetSocketKey();
    if (socketKey->NetType != NetworkType::TcpListen)
        return;

    auto pPlayerCollector = GetComponent<PlayerCollectorComponent>();
    pPlayerCollector->RemovePlayerBySocket(pPacket->GetSocketKey()->Socket);
}

void Account::SocketDisconnect(std::string account, NetIdentify* pIdentify)
{
    Proto::AccountCheckRs protoResult;
    protoResult.set_return_code(Proto::AccountCheckReturnCode::ARC_LOGGING);
    MessageSystemHelp::SendPacket(Proto::MsgId::C2L_AccountCheckRs, protoResult, pIdentify);

    // 关闭网络
    MessageSystemHelp::DispatchPacket(Proto::MsgId::MI_NetworkRequestDisconnect, pIdentify);
}

void Account::HandleAccountCheck(Packet* pPacket)
{
    auto protoCheck = pPacket->ParseToProto<Proto::AccountCheck>();
    //std::cout << "account check account:" << protoCheck.account() << pPacket << std::endl;

    auto pPlayerCollector = GetComponent<PlayerCollectorComponent>();

    // 相同账号正在登录
    auto pPlayer = pPlayerCollector->GetPlayerByAccount(protoCheck.account());
    if (pPlayer != nullptr)
    {
        LOG_WARN("account check failed. same account:" << protoCheck.account().c_str() << ". " << pPlayer);
        SocketDisconnect(protoCheck.account(), pPacket);
        return;
    }

    // 更新信息
    pPlayer = pPlayerCollector->AddPlayer(pPacket, protoCheck.account());
    if (pPlayer == nullptr)
    {
        LOG_WARN("account check failed. same socket. " << pPacket);
        SocketDisconnect(protoCheck.account(), pPacket);
        return;
    }

    pPlayer->AddComponent<PlayerComponentAccount>(protoCheck.password());

#ifdef LOG_TRACE_COMPONENT_OPEN
    ComponentHelp::GetTraceComponent()->TraceAccount(protoCheck.account(), pPacket->GetSocketKey()->Socket);
#endif

    Proto::AccountQueryOnlineToRedis protoToRedis;
    protoToRedis.set_account(pPlayer->GetAccount().c_str());
    MessageSystemHelp::DispatchPacket(Proto::MsgId::MI_AccountQueryOnlineToRedis, protoToRedis, nullptr);
}


void Account::HandleAccountQueryOnlineToRedisRs(Packet* pPacket)
{
    auto protoRs = pPacket->ParseToProto<Proto::AccountQueryOnlineToRedisRs>();

    auto pPlayer = GetComponent<PlayerCollectorComponent>()->GetPlayerByAccount(protoRs.account());
    if (pPlayer == nullptr)
        return;

    if (protoRs.return_code() != Proto::AccountQueryOnlineToRedisRs::SOTR_Offline)
    {
        //LOG_WARN("check [3/3]. account is online. " << protoRs.account().c_str());

        // 结果给客户端
        Proto::AccountCheckRs protoResult;
        protoResult.set_return_code(Proto::AccountCheckReturnCode::ARC_ONLINE);
        MessageSystemHelp::SendPacket(Proto::MsgId::C2L_AccountCheckRs, protoResult, pPlayer);
        return;
    }

    // 在线组件
    pPlayer->AddComponent<PlayerComponentOnlineInLogin>(pPlayer->GetAccount());

    // 验证账号，发起一个Http请求
    TagValue tagValue { pPlayer->GetAccount(), 0 };
    MessageSystemHelp::CreateConnect(NetworkType::HttpConnector, TagType::Account, tagValue, _httpIp.c_str(), _httpPort);
}

void Account::HandleQueryPlayerListRs(Packet* pPacket)
{
    auto protoRs = pPacket->ParseToProto<Proto::PlayerList>();
    std::string account = protoRs.account();

    auto pPlayerCollector = GetComponent<PlayerCollectorComponent>();

    const auto pPlayer = pPlayerCollector->GetPlayerByAccount(account);
    if (pPlayer == nullptr)
    {
        LOG_ERROR("HandleQueryPlayerLists. pPlayer == nullptr. account:" << account.c_str());
        return;
    }

    if (protoRs.player_size() > 0)
    {
        auto pListObj = pPlayer->GetComponent<PlayerComponentProtoList>();
        if (pListObj == nullptr)
            pListObj = pPlayer->AddComponent<PlayerComponentProtoList>();

        pListObj->Parse(protoRs);
    }

    //LOG_DEBUG("HandlePlayerListToDBRs account:" << account.c_str() << ", player size:" << protoRs.player_size());

    // 将结果转送给客户端
    MessageSystemHelp::SendPacket(Proto::MsgId::L2C_PlayerList, protoRs, pPlayer);
}

void Account::HandleCreatePlayer(Packet* pPacket)
{
    auto pPlayerCollector = GetComponent<PlayerCollectorComponent>();

    auto protoCreate = pPacket->ParseToProto<Proto::CreatePlayer>();
    const auto pPlayer = pPlayerCollector->GetPlayerBySocket(pPacket->GetSocketKey()->Socket);
    if (pPlayer == nullptr)
    {
        LOG_ERROR("HandleCreatePlayer. pPlayer == nullptr. socket:" << pPacket->GetSocketKey()->Socket);
        return;
    }

    std::string account = pPlayer->GetAccount();
    LOG_DEBUG("create. account:" << account.c_str() << " name:" << protoCreate.name().c_str() << " gender:" << (int)protoCreate.gender());

    Proto::CreatePlayerToDB proto2Db;
    proto2Db.set_account(account.c_str());

    auto pProtoPlayer = proto2Db.mutable_player();
    pProtoPlayer->set_sn(Global::GetInstance()->GenerateSN());
    pProtoPlayer->set_name(protoCreate.name());
    pProtoPlayer->mutable_base()->set_gender(protoCreate.gender());

    MessageSystemHelp::SendPacket(Proto::MsgId::L2DB_CreatePlayer, proto2Db, APP_DB_MGR);
}

void Account::HandleCreatePlayerRs(Packet* pPacket)
{
    auto protoRs = pPacket->ParseToProto<Proto::CreatePlayerToDBRs>();
    std::string account = protoRs.account();

    auto pPlayerCollector = GetComponent<PlayerCollectorComponent>();

    auto pPlayer = pPlayerCollector->GetPlayerBySocket(pPacket->GetSocketKey()->Socket);
    if (pPlayer == nullptr)
    {
        LOG_ERROR("HandleCreatePlayerToDBRs. pPlayer == nullptr. account:" << account.c_str());
        return;
    }

    Proto::CreatePlayerRs createProto;
    createProto.set_return_code(protoRs.return_code());
    MessageSystemHelp::SendPacket(Proto::MsgId::C2L_CreatePlayerRs, createProto, pPlayer);
}

void Account::HandleSelectPlayer(Packet* pPacket)
{
    Proto::SelectPlayerRs protoRs;
    protoRs.set_return_code(Proto::SelectPlayerRs::SPRC_OK);

    auto proto = pPacket->ParseToProto<Proto::SelectPlayer>();
    auto pPlayerMgr = GetComponent<PlayerCollectorComponent>();
    auto pPlayer = pPlayerMgr->GetPlayerBySocket(pPacket->GetSocketKey()->Socket);
    if (pPlayer == nullptr)
    {
        LOG_ERROR("HandleSelectPlayer. pPlayer == nullptr");
        return;
    }

    uint64 selectPlayerSn = proto.player_sn();

    auto pPlayerLoginInfo = pPlayer->GetComponent<PlayerComponentAccount>();
    pPlayerLoginInfo->SetSelectPlayerSn(selectPlayerSn);

    do
    {
        if (pPlayer == nullptr)
        {
            protoRs.set_return_code(Proto::SelectPlayerRs::SPRC_NotFound);
            LOG_ERROR("HandleSelectPlayer. pPlayer == nullptr. " << pPacket);
            break;
        }

        auto pSubCompoent = pPlayer->GetComponent<PlayerComponentProtoList>();
        if (pSubCompoent == nullptr)
        {
            protoRs.set_return_code(Proto::SelectPlayerRs::SPRC_NotFound);
            LOG_ERROR("HandleSelectPlayer. pPlayer == nullptr. " << pPacket);
            break;
        }

        std::stringstream* pOpStream = pSubCompoent->GetProto(selectPlayerSn);
        if (pOpStream == nullptr)
        {
            protoRs.set_return_code(Proto::SelectPlayerRs::SPRC_NotFound);
            LOG_ERROR("HandleSelectPlayer. can't find player sn:" << selectPlayerSn);
            break;
        }

        pPlayer->ParseFromStream(selectPlayerSn, pOpStream);
    } while (false);

    if (Proto::SelectPlayerRs::SPRC_OK != protoRs.return_code())
    {
        MessageSystemHelp::SendPacket(Proto::MsgId::C2L_SelectPlayerRs, protoRs, pPlayer);
        return;
    }

    //  请求token
    RequestToken(pPlayer);
}

void Account::RequestToken(Player* pPlayer) const
{
    // 请求一个Token
    Proto::LoginTokenToRedis protoToken;
    protoToken.set_account(pPlayer->GetAccount().c_str());

    const auto pLoginInfo = pPlayer->GetComponent<PlayerComponentAccount>();
    protoToken.set_player_sn(pLoginInfo->GetSelectPlayerSn());
    MessageSystemHelp::DispatchPacket(Proto::MsgId::MI_LoginTokenToRedis, protoToken, nullptr);
}

void Account::HandleHttpOuterResponse(Packet* pPacket)
{    
    auto pTagValue = pPacket->GetTagKey()->GetTagValue(TagType::Account);
    if (pTagValue == nullptr)
    {
        LOG_ERROR("HandleHttpOuterResponse. can't find player.");
        return;
    }

    auto account = pTagValue->KeyStr;

    auto pPlayerCollector = GetComponent<PlayerCollectorComponent>();
    auto pPlayer = pPlayerCollector->GetPlayerByAccount(account);
    if (pPlayer == nullptr)
    {
        LOG_ERROR("http out response. but can't find player. account:" << account.c_str() << pPacket);
        return;
    }

    auto protoHttp = pPacket->ParseToProto<Proto::Http>();
    Proto::AccountCheckReturnCode rsCode = Proto::AccountCheckReturnCode::ARC_TIMEOUT;
    int statusCode = protoHttp.status_code();
    if (statusCode == 200)
    {
        auto response = protoHttp.body();
        const Json::CharReaderBuilder readerBuilder;
        Json::CharReader* jsonReader = readerBuilder.newCharReader();

        Json::Value value;
        JSONCPP_STRING errs;


        const bool ok = jsonReader->parse(response.data(), response.data() + response.size(), &value, &errs);
        if (ok && errs.empty())
        {
            rsCode = ProcessMsg(value);
        }
        else
        {
            LOG_ERROR("json parse failed. " << response.c_str());
        }

        delete jsonReader;
    }

    // 不论成功，关闭http连接
    MessageSystemHelp::DispatchPacket(Proto::MsgId::MI_NetworkRequestDisconnect, pPacket);

    //通知客户端进入lobby地图
    auto pResMsg = ResourceHelp::GetResourceManager();
    auto pRolesMap = pResMsg->Worlds->GetRolesMap();
    if (pRolesMap != nullptr)
    {
        Proto::EnterWorld protoEnterWorld;
        protoEnterWorld.set_world_id(pRolesMap->GetId());
        MessageSystemHelp::SendPacket(Proto::MsgId::S2C_EnterWorld, protoEnterWorld, pPlayer);
    }
    else
    {
        LOG_ERROR("config error. not roles map.");
    }

    // 验证成功，向DB发起查询
    if (rsCode == Proto::AccountCheckReturnCode::ARC_OK)
    {
        Proto::QueryPlayerList protoQuery;
        protoQuery.set_account(pPlayer->GetAccount().c_str());
        MessageSystemHelp::SendPacket(Proto::MsgId::L2DB_QueryPlayerList, protoQuery, APP_DB_MGR);
    }
    else
    {
        Proto::AccountCheckRs protoResult;
        protoResult.set_return_code(rsCode);
        MessageSystemHelp::SendPacket(Proto::MsgId::C2L_AccountCheckRs, protoResult, pPlayer);
    }
}

Proto::AccountCheckReturnCode Account::ProcessMsg(Json::Value value) const
{
    Proto::AccountCheckReturnCode code = Proto::AccountCheckReturnCode::ARC_UNKONWN;
    const int httpcode = value["returncode"].asInt();
    if (httpcode == 0)
        code = Proto::AccountCheckReturnCode::ARC_OK;
    else if (httpcode == 2)
        code = Proto::AccountCheckReturnCode::ARC_NOT_FOUND_ACCOUNT;
    else if (httpcode == 3)
        code = Proto::AccountCheckReturnCode::ARC_PASSWORD_WRONG;

    return code;
}

void Account::HandleTokenToRedisRs(Packet* pPacket)
{
    auto protoRs = pPacket->ParseToProto<Proto::LoginTokenToRedisRs>();
    const auto token = protoRs.token();

    Player* pPlayer = GetComponent<PlayerCollectorComponent>()->GetPlayerByAccount(protoRs.account());
    if (pPlayer == nullptr)
    {
        LOG_WARN("can't find player. account:" << protoRs.account().c_str());
        return;
    }

    Proto::GameToken protoToken;
    AppInfo info;
    if (!GetOneApp(APP_GAME, &info))
    {
        protoToken.set_return_code(Proto::GameToken_ReturnCode_GameToken_NO_GAME);
    }
    else
    {
        protoToken.set_return_code(Proto::GameToken_ReturnCode_GameToken_OK);

        protoToken.set_ip(info.Ip.c_str());
        protoToken.set_port(info.Port);
        protoToken.set_token(token);
    }

    MessageSystemHelp::SendPacket(Proto::MsgId::L2C_GameToken, protoToken, pPlayer);

}
