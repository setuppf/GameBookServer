
#include "mysql_connector.h"
#include "libserver/log4_help.h"

void MysqlConnector::RegisterMsgFunction()
{
    auto pMsgCallBack = new MessageCallBackFunction();
    AttachCallBackHandler(pMsgCallBack);

    pMsgCallBack->RegisterFunction(Proto::MsgId::L2DB_QueryPlayerList, BindFunP1(this, &MysqlConnector::HandleQueryPlayerList));
    pMsgCallBack->RegisterFunction(Proto::MsgId::L2DB_CreatePlayer, BindFunP1(this, &MysqlConnector::HandleCreatePlayer));

    pMsgCallBack->RegisterFunction(Proto::MsgId::G2DB_SavePlayer, BindFunP1(this, &MysqlConnector::HandleSavePlayer));
    pMsgCallBack->RegisterFunction(Proto::MsgId::G2DB_QueryPlayer, BindFunP1(this, &MysqlConnector::HandleQueryPlayer));
}

void MysqlConnector::HandleQueryPlayerList(Packet* pPacket)
{
    auto protoQuery = pPacket->ParseToProto<Proto::QueryPlayerList>();
    QueryPlayerList(protoQuery.account(), pPacket->GetSocket());
}

void MysqlConnector::QueryPlayerList(std::string account, SOCKET socket)
{
    my_ulonglong affected_rows;
    std::string sql = strutil::format("select sn, name, base, item, misc from player where account = '%s'", account.c_str());
    if (!Query(sql.c_str(), affected_rows))
    {
        LOG_ERROR("!!! Failed. MysqlConnector::HandleQueryPlayerList. sql:" << sql.c_str());
        return;
    }

    Proto::PlayerList protoRs;
    protoRs.set_account(account.c_str());

    Proto::PlayerBase protoBase;
    if (affected_rows > 0)
    {
        std::string tempStr;
        MYSQL_ROW row;
        while ((row = Fetch()))
        {
            auto pProtoPlayer = protoRs.add_player();
            pProtoPlayer->set_sn(GetUint64(row, 0));
            pProtoPlayer->set_name(GetString(row, 1));

            GetBlob(row, 2, tempStr);
            protoBase.ParseFromString(tempStr);
            pProtoPlayer->set_level(protoBase.level());
            pProtoPlayer->set_gender(protoBase.gender());
        }
    }

    LOG_DEBUG("player list. account:" << account.c_str() << " player list size:" << protoRs.player_size() << " socket:" << socket);

    // 没有找到也需要返回pResultPacket
    SendPacket(Proto::MsgId::L2DB_QueryPlayerListRs, socket, protoRs);
}

void MysqlConnector::HandleQueryPlayer(Packet* pPacket)
{
    auto protoQuery = pPacket->ParseToProto<Proto::QueryPlayer>();

    my_ulonglong affected_rows;
    std::string sql = strutil::format("select sn, name, account, base, item, misc from player where sn = %llu", protoQuery.player_sn());
    if (!Query(sql.c_str(), affected_rows))
    {
        LOG_ERROR("!!! Failed. MysqlConnector::HandleQueryPlayer. sql:" << sql.c_str());
        return;
    }

    Proto::QueryPlayerRs protoRs;
    if (affected_rows > 0)
    {
        std::string tempStr;
        MYSQL_ROW row;
        if ((row = Fetch()))
        {
            Proto::Player* pProtoPlayer = protoRs.mutable_player();
            pProtoPlayer->set_sn(GetUint64(row, 0));
            pProtoPlayer->set_name(GetString(row, 1));
            protoRs.set_account(GetString(row, 2));

            GetBlob(row, 3, tempStr);
            pProtoPlayer->mutable_base()->ParseFromString(tempStr);

            GetBlob(row, 5, tempStr);
            pProtoPlayer->mutable_misc()->ParseFromString(tempStr);
        }
    }

    //LOG_DEBUG("player  account:" << protoQuery.account().c_str() << " player list size:" << protoRs.player_size() << " socket:" << pPacket->GetSocket());
    SendPacket(Proto::MsgId::G2DB_QueryPlayerRs, pPacket->GetSocket(), protoRs);
}

void MysqlConnector::HandleCreatePlayer(Packet* pPacket)
{
    auto protoCreate = pPacket->ParseToProto<Proto::CreatePlayerToDB>();

    auto protoPlayer = protoCreate.player();

    // todo 是否有重名
    DatabaseStmt* stmt = GetStmt(DatabaseStmtKey::StmtCreate);
    if (stmt == nullptr)
        return;

    LOG_INFO("HandlePlayerCreate sn:" << protoPlayer.sn() << " account:" << protoCreate.account().c_str() << " name:" << protoPlayer.name().c_str());

    // create
    ClearStmtParam(stmt);
    AddParamUint64(stmt, protoPlayer.sn());
    AddParamStr(stmt, protoCreate.account().c_str());
    AddParamStr(stmt, protoPlayer.name().c_str());

    Proto::CreatePlayerToDBRs protoRs;
    protoRs.set_account(protoCreate.account().c_str());
    protoRs.set_return_code(Proto::CreatePlayerReturnCode::CPR_Unkonwn);

    if (ExecuteStmt(stmt))
    {
        protoRs.set_return_code(Proto::CreatePlayerReturnCode::CPR_Create_OK);
    }

    // 如果创建成功，将player list发送到客户端
    // 如果失败，将失败码返回到客户端
    if (protoRs.return_code() == Proto::CreatePlayerReturnCode::CPR_Create_OK)
    {
        QueryPlayerList(protoCreate.account(), pPacket->GetSocket());
    }
    else
    {
        SendPacket(Proto::MsgId::L2DB_CreatePlayerRs, pPacket->GetSocket(), protoRs);
    }
}

void MysqlConnector::HandleSavePlayer(Packet* pPacket)
{
    auto proto = pPacket->ParseToProto<Proto::SavePlayer>();
    DatabaseStmt* stmt = GetStmt(DatabaseStmtKey::StmtSave);
    if (stmt == nullptr)
        return;

    //LOG_INFO("HandleSavePlayer sn:" << proto.player_sn());
    Proto::Player protoPlayer = proto.player();
    OnSavePlayer(stmt, protoPlayer);
}

bool MysqlConnector::OnSavePlayer(DatabaseStmt* stmtSave, Proto::Player& protoPlayer)
{
    ClearStmtParam(stmtSave);

    std::string baseStr;
    protoPlayer.base().SerializeToString(&baseStr);
    AddParamBlob(stmtSave, (void*)baseStr.c_str(), (int)baseStr.size());

    std::string miscStr;
    protoPlayer.misc().SerializeToString(&miscStr);
    AddParamBlob(stmtSave, (void*)miscStr.c_str(), (int)miscStr.size());

    AddParamUint64(stmtSave, protoPlayer.sn());

    if (!ExecuteStmt(stmtSave))
    {
        LOG_ERROR("save player failed. player sn:" << protoPlayer.sn());
        return false;
    }

    return true;
}
