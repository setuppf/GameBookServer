#include "mysql_connector.h"
#include "libserver/log4_help.h"
#include <thread>

#define MysqlPingTime 2 * 60

void MysqlConnector::AwakeFromPool()
{
    mysql_thread_init();

    LOG_DEBUG("MysqlConnector::Awake. id:" << std::this_thread::get_id());

    auto pYaml = Yaml::GetInstance();
    auto pConfig = pYaml->GetConfig(APP_DB_MGR);
    auto pDbCfig = dynamic_cast<DBMgrConfig*>(pConfig);

    _pDbCfg = pDbCfig->GetDBConfig(DBMgrConfig::DBTypeMysql);
    if (_pDbCfg == nullptr)
    {
        LOG_ERROR("Failed to get mysql config.");
        return;
    }

    InitMessageComponent();
    Connect();

    // check ping     
    //AddTimeHeapFunction(0, MysqlPingTime, false, 0, BindFunP0(this, &MysqlConnector::CheckPing));
}

void MysqlConnector::CheckPing()
{
    //LOG_DEBUG( "check mysql ping." );

    if (_isRunning)
    {
        mysql_ping(_pMysql);
        if (CheckMysqlError() != 0)
        {
            Disconnect();
        }
    }

    if (!_isRunning)
    {
        ReConnect();
    }
}

void MysqlConnector::BackToPool()
{
    Disconnect();
    mysql_thread_end();
}

bool MysqlConnector::Connect()
{
    if (!ConnectInit())
        return false;

    LOG_DEBUG("MysqlConnector::Connect. " << _pDbCfg->Ip.c_str() << ":" << _pDbCfg->Port << " starting... id:" << std::this_thread::get_id());

    mysql_real_connect(_pMysql, _pDbCfg->Ip.c_str(), _pDbCfg->User.c_str(), _pDbCfg->Password.c_str(), _pDbCfg->DatabaseName.c_str(), _pDbCfg->Port, nullptr, CLIENT_FOUND_ROWS);

    int mysqlerrno = CheckMysqlError();
    if (mysqlerrno > 0)
    {
        Disconnect();
        return false;
    }

    // 不需要关闭自动提交，底层会START TRANSACTION之后再COMMIT
    // mysql_autocommit(mysql(), 0);

    mysql_ping(_pMysql);
    mysqlerrno = CheckMysqlError();
    if (mysqlerrno > 0)
    {
        Disconnect();
        return false;
    }

    LOG_DEBUG("MysqlConnector::Connect: successfully! addr:" << _pDbCfg->Ip.c_str() << ":" << _pDbCfg->Port);

    InitStmts();

    _isRunning = true;

    return true;
}

void MysqlConnector::ReConnect()
{
    if (!ConnectInit())
        return;

    LOG_DEBUG("MysqlConnector::ReConnect. " << _pDbCfg->Ip.c_str() << ":" << _pDbCfg->Port << " starting...");

    if (nullptr == mysql_real_connect(_pMysql, _pDbCfg->Ip.c_str(), _pDbCfg->User.c_str(), _pDbCfg->Password.c_str(), _pDbCfg->DatabaseName.c_str(), _pDbCfg->Port, nullptr, CLIENT_FOUND_ROWS))
    {
        LOG_ERROR("MysqlConnector::ReConnect failed. " << _pDbCfg->Ip.c_str() << ":" << _pDbCfg->Port << " starting...");
        CheckMysqlError();
        return;
    }

    mysql_select_db(_pMysql, _pDbCfg->DatabaseName.c_str());
    if (CheckMysqlError() != 0)
    {
        Disconnect();
        return;
    }

    LOG_DEBUG("MysqlConnector::ReConnect: successfully! addr:" << _pDbCfg->Ip.c_str() << ":" << _pDbCfg->Port);

    InitStmts();

    _isRunning = true;
}

void MysqlConnector::Disconnect()
{
    CleanStmts();
    MysqlBase::Disconnect();
}

void MysqlConnector::InitStmts()
{
    DatabaseStmt* stmt = CreateStmt("insert into player ( sn, account, name, savetime, createtime ) value ( ?, ?, ?, now(), now() )");
    _mapStmt.insert(std::make_pair(DatabaseStmtKey::StmtCreate, stmt));

    stmt = CreateStmt("update player set base=?, misc=?,savetime=now() where sn = ?");
    _mapStmt.insert(std::make_pair(DatabaseStmtKey::StmtSave, stmt));

    LOG_DEBUG("\tMysqlConnector::InitStmts successfully!");
}

void MysqlConnector::CleanStmts()
{
    for (auto one : _mapStmt)
    {
        one.second->Close();
    }

    _mapStmt.clear();
}

DatabaseStmt* MysqlConnector::GetStmt(DatabaseStmtKey stmtKey)
{
    const auto iter = _mapStmt.find(stmtKey);
    if (iter == _mapStmt.end())
        return nullptr;

    return iter->second;
}

DatabaseStmt* MysqlConnector::CreateStmt(const char* sql) const
{
    int str_len = (int)strlen(sql);
    DatabaseStmt* stmt = new DatabaseStmt();
    int param_count = 0;
    stmt->stmt = mysql_stmt_init(_pMysql);
    if (mysql_stmt_prepare(stmt->stmt, sql, str_len) != 0)
    {
        LOG_ERROR("create_stmt error, " << LOG4CPLUS_C_STR_TO_TSTRING(mysql_stmt_error(stmt->stmt)));
        return nullptr;
    }

    for (auto i = 0; i < str_len; i++)
    {
        if ((sql[i] == '?') || (sql[i] == '@'))
            param_count++;
    }

    if (param_count > 0)
    {
        stmt->bind = new MYSQL_BIND[param_count];
        memset(stmt->bind, 0, sizeof(MYSQL_BIND) * param_count);
        stmt->bind_buffer = new char[MAX_BIND_BUFFER];
    }
    else
    {
        stmt->bind = nullptr;
        stmt->bind_buffer = nullptr;
    }

    return stmt;
}

void MysqlConnector::ClearStmtParam(DatabaseStmt* stmt)
{
    stmt->bind_index = 0;
    stmt->bind_buffer_index = 0;
}

bool MysqlConnector::ExecuteStmt(DatabaseStmt* stmt, my_ulonglong& affected_rows)
{
    MYSQL_STMT* st = stmt->stmt;
    if (NULL != stmt->bind)
        mysql_stmt_bind_param(stmt->stmt, stmt->bind);
    if (mysql_stmt_execute(st) != 0)
    {
        LOG_ERROR("execute_stmt error, " << LOG4CPLUS_C_STR_TO_TSTRING(mysql_stmt_error(st)));
        return false;
    }

    affected_rows = mysql_stmt_affected_rows(st);
    return true;
}

bool MysqlConnector::ExecuteStmt(DatabaseStmt* stmt)
{
    MYSQL_STMT* st = stmt->stmt;
    if (NULL != stmt->bind)
        mysql_stmt_bind_param(stmt->stmt, stmt->bind);
    if (mysql_stmt_execute(st) != 0)
    {
        LOG_ERROR("execute_stmt error, " << LOG4CPLUS_C_STR_TO_TSTRING(mysql_stmt_error(st)));
        return false;
    }

    return true;
}

void MysqlConnector::AddParamBlob(DatabaseStmt* stmt, void* val, int size)
{
    MYSQL_BIND* pBind = &stmt->bind[stmt->bind_index];

    pBind->buffer_type = MYSQL_TYPE_BLOB;
    pBind->buffer = val;
    pBind->length = reinterpret_cast<unsigned long*>(&stmt->bind_buffer[stmt->bind_buffer_index + sizeof(void*)]);

    *(pBind->length) = size;
    pBind->buffer_length = size;

    stmt->bind_index++;
    stmt->bind_buffer_index += (sizeof(void*) + sizeof(unsigned long*));
}

void MysqlConnector::AddParamUint64(DatabaseStmt* stmt, uint64 val)
{
    MYSQL_BIND* pBind = &stmt->bind[stmt->bind_index];

    pBind->buffer_type = MYSQL_TYPE_LONGLONG;
    pBind->buffer = &stmt->bind_buffer[stmt->bind_buffer_index];
    pBind->is_unsigned = true;

    *static_cast<uint64*>(pBind->buffer) = val;

    stmt->bind_index++;
    stmt->bind_buffer_index += sizeof(uint64);
}

void MysqlConnector::AddParamInt(DatabaseStmt* stmt, int val)
{
    MYSQL_BIND* pBind = &stmt->bind[stmt->bind_index];

    pBind->buffer_type = MYSQL_TYPE_LONG;
    pBind->buffer = &stmt->bind_buffer[stmt->bind_buffer_index];
    pBind->is_unsigned = false;

    *static_cast<long*>(pBind->buffer) = val;

    stmt->bind_index++;
    stmt->bind_buffer_index += sizeof(long);
}

void MysqlConnector::AddParamStr(DatabaseStmt* stmt, const char* val)
{
    MYSQL_BIND* pBind = &stmt->bind[stmt->bind_index];
    int len = strlen(val);
    if (len >= MAX_BIND_STR)
    {
        LOG_ERROR("add_str_param error, str size:" << len << " out of limit:" << MAX_BIND_STR);
        len = MAX_BIND_STR - 1;
    }

    pBind->buffer_type = MYSQL_TYPE_STRING;
    pBind->buffer = &stmt->bind_buffer[stmt->bind_buffer_index];
    pBind->length = (unsigned long*)& stmt->bind_buffer[stmt->bind_buffer_index + len + 1];

    engine_strncpy((char*)pBind->buffer, len + 1, val, len + 1);
    *(pBind->length) = len;
    pBind->buffer_length = len;

    stmt->bind_index++;
    stmt->bind_buffer_index += (len + 1 + sizeof(unsigned long*));
}
