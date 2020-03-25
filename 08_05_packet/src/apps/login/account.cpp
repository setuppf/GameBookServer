#include "account.h"
#include "libserver/common.h"
#include "libserver/thread_mgr.h"
#include "libserver/log4_help.h"
#include "libserver/message_component.h"
#include "libserver/message_system_help.h"
#include "libserver/component_help.h"

#include "http_request_account.h"

void Account::Awake()
{
    auto pMsgCallBack = new MessageCallBackFunction();
    AddComponent<MessageComponent>(pMsgCallBack);

    pMsgCallBack->RegisterFunction(Proto::MsgId::C2L_AccountCheck, BindFunP1(this, &Account::HandleAccountCheck));
    pMsgCallBack->RegisterFunction(Proto::MsgId::MI_AccountCheckToHttpRs, BindFunP1(this, &Account::HandleAccountCheckToHttpRs));
    pMsgCallBack->RegisterFunction(Proto::MsgId::C2L_CreatePlayer, BindFunP1(this, &Account::HandleCreatePlayer));
    pMsgCallBack->RegisterFunction(Proto::MsgId::L2DB_CreatePlayerRs, BindFunP1(this, &Account::HandleCreatePlayerRs));

    // db
    pMsgCallBack->RegisterFunction(Proto::MsgId::L2DB_QueryPlayerListRs, BindFunP1(this, &Account::HandleQueryPlayerListRs));

    // 处理断线
    pMsgCallBack->RegisterFunction(Proto::MsgId::MI_NetworkDisconnect, BindFunP1(this, &Account::HandleNetworkDisconnect));
}

void Account::BackToPool()
{
    _playerMgr.BackToPool();
}

void Account::HandleNetworkDisconnect(Packet* pPacket)
{
    //std::cout << "player offline. socket:" << pPacket->GetSocket() << std::endl;
    _playerMgr.RemovePlayer(pPacket->GetSocket());
}

void Account::HandleAccountCheck(Packet* pPacket)
{
    auto protoCheck = pPacket->ParseToProto<Proto::AccountCheck>();
    const auto socket = pPacket->GetSocket();

    //std::cout << "account check account:" << protoCheck.account() << " socket:" << socket << std::endl;

    // 相同账号正在登录
    auto pPlayer = _playerMgr.GetPlayer(protoCheck.account());
    if (pPlayer != nullptr)
    {
        Proto::AccountCheckRs protoResult;
        protoResult.set_return_code(Proto::AccountCheckReturnCode::ARC_LOGGING);
        MessageSystemHelp::SendPacket(Proto::MsgId::C2L_AccountCheckRs, socket, protoResult);

        std::cout << "account check failed. same account:" << protoCheck.account() << " socket:" << socket << std::endl;

        // 关闭网络
        MessageSystemHelp::DispatchPacket(Proto::MsgId::MI_NetworkRequestDisconnect, socket);
        return;
    }

    // 更新信息
    _playerMgr.AddPlayer(socket, protoCheck.account(), protoCheck.password());

    // 验证账号(HTTP)
    ThreadMgr::GetInstance()->CreateComponent<HttpRequestAccount>(protoCheck.account(), protoCheck.password());
}

void Account::HandleAccountCheckToHttpRs(Packet* pPacket)
{
    auto proto = pPacket->ParseToProto<Proto::AccountCheckToHttpRs>();

    auto pPlayer = _playerMgr.GetPlayer(proto.account());
    if (pPlayer == nullptr)
    {
        std::cout << "can't find player. account:" << proto.account().c_str() << std::endl;
        return;
    }

    //std::cout << "account check result account:" << proto.account() << " rs:" << proto.return_code() << " socket:" << pPlayer->GetSocket() << std::endl;

    Proto::AccountCheckRs protoResult;
    protoResult.set_return_code(proto.return_code());
    MessageSystemHelp::SendPacket(Proto::MsgId::C2L_AccountCheckRs, pPlayer->GetSocket(), protoResult);

    // 验证成功，向DB发起查询
    if (proto.return_code() == Proto::AccountCheckReturnCode::ARC_OK)
    {
        Proto::QueryPlayerList protoQuery;
        protoQuery.set_account(pPlayer->GetAccount().c_str());
        MessageSystemHelp::SendPacket(Proto::MsgId::L2DB_QueryPlayerList, protoQuery, APP_DB_MGR);
    }
}

void Account::HandleQueryPlayerListRs(Packet* pPacket)
{
    auto protoRs = pPacket->ParseToProto<Proto::PlayerList>();
    std::string account = protoRs.account();

    auto pPlayer = _playerMgr.GetPlayer(account);
    if (pPlayer == nullptr)
    {
        LOG_ERROR("HandleQueryPlayerLists. pPlayer == nullptr. account:" << account.c_str());
        return;
    }

    //LOG_DEBUG("HandlePlayerListToDBRs account:" << account.c_str() << ", player size:" << protoRs.player_size());

    // 将结果转送给客户端
    MessageSystemHelp::SendPacket(Proto::MsgId::L2C_PlayerList, pPlayer->GetSocket(), protoRs);
}

void Account::HandleCreatePlayer(Packet* pPacket)
{
    auto protoCreate = pPacket->ParseToProto<Proto::CreatePlayer>();
    auto pPlayer = _playerMgr.GetPlayer(pPacket->GetSocket());
    if (pPlayer == nullptr)
    {
        LOG_ERROR("HandleCreatePlayer. pPlayer == nullptr. socket:" << pPacket->GetSocket());
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

    auto pPlayer = _playerMgr.GetPlayer(pPacket->GetSocket());
    if (pPlayer == nullptr)
    {
        LOG_ERROR("HandleCreatePlayerToDBRs. pPlayer == nullptr. account:" << account.c_str());
        return;
    }

    Proto::CreatePlayerRs createProto;
    createProto.set_return_code(protoRs.return_code());
    MessageSystemHelp::SendPacket(Proto::MsgId::C2L_CreatePlayerRs, pPlayer->GetSocket(), createProto);
}
