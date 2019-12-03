#include "redis_connector.h"
#include "log4_help.h"
#include "common.h"

#include "util_string.h"
#include "thread_mgr.h"
#include "yaml.h"
#include "component_help.h"

void RedisConnector::Awake()
{
    RegisterMsgFunction();

    auto pYaml = ComponentHelp::GetYaml();
    const auto pConfig = pYaml->GetConfig(APP_DB_MGR);
    auto pDBConfig = dynamic_cast<DBMgrConfig*>(pConfig);

    auto pRedisConfig = pDBConfig->GetDBConfig(DBMgrConfig::DBTypeRedis);
    if (pRedisConfig == nullptr)
    {
        LOG_ERROR("Init failed. get redis config is failed.");
        return;
    }

    _ip = pRedisConfig->Ip;
    _port = pRedisConfig->Port;

    // time function
    AddTimer(0, 2 * 60, false, 0, BindFunP0(this, &RedisConnector::CheckPing));

    Connect();
}

void RedisConnector::BackToPool()
{
    Disconnect();
}

void RedisConnector::CheckPing()
{
    //LOG_DEBUG( "check redis ping." );

    if (Ping())
        return;

    Disconnect();
    Connect();
}

bool RedisConnector::Ping() const
{
    if (_pRedisContext == nullptr)
        return false;

    redisReply* pRedisReply = static_cast<redisReply*>(redisCommand(_pRedisContext, "PING"));

    if (nullptr == pRedisReply)
    {
        LOG_ERROR("RedisConnector::ping: errno=" << _pRedisContext->err << " error=" << _pRedisContext->errstr);
        return false;
    }

    if (!(pRedisReply->type == REDIS_REPLY_STATUS && strncmp(pRedisReply->str, "PONG", 4) == 0))
    {
        LOG_ERROR("RedisConnector::ping: errno=" << _pRedisContext->err << " error=" << _pRedisContext->errstr);

        freeReplyObject(pRedisReply);
        return false;
    }

    freeReplyObject(pRedisReply);
    return true;
}

bool RedisConnector::Connect()
{
    LOG_DEBUG("RedisConnector::Connect. " << _ip.c_str() << ":" << _port << ". starting...");

    const struct timeval outTime = { 1, 500000 };  // 1.5 seconds
    redisContext* c = redisConnectWithTimeout(_ip.c_str(), _port, outTime);
    if (c == nullptr || c->err)
    {
        if (c)
        {
            LOG_ERROR("RedisConnector::Connect. errno=" << c->err << ", error=" << c->errstr);
            redisFree(c);
        }
        else
        {
            LOG_ERROR("RedisConnector error: can't allocate redis context");
        }
        return false;
    }

    // 选择数据库(1)号数据库
    int db_index = 1;
    std::string sql = strutil::format("select %d", db_index);
    redisReply* pRedisReply = static_cast<redisReply*>(redisCommand(c, sql.c_str()));

    if (nullptr == pRedisReply)
    {
        LOG_ERROR("RedisConnector::Connect. errno=" << c->err << ", error=" << c->errstr);

        redisFree(c);
        return false;
    }

    if (!(pRedisReply->type == REDIS_REPLY_STATUS && strncmp(pRedisReply->str, "OK", 2) == 0))
    {
        LOG_ERROR("RedisConnector::Connect. errno=" << c->err << ", error=" << c->errstr);

        freeReplyObject(pRedisReply);
        redisFree(c);
        return false;
    }

    freeReplyObject(pRedisReply);
    _pRedisContext = c;

    LOG_DEBUG("\tRedisConnector::Connect: successfully!");
    return Ping();
}

bool RedisConnector::Disconnect()
{
    if (_pRedisContext != nullptr)
    {
        redisFree(_pRedisContext);
        _pRedisContext = nullptr;
    }

    return true;
}

bool RedisConnector::Setex(std::string key, std::string value, const int timeout) const
{
    const std::string command = strutil::format("SETEX %s %d %s", key.c_str(), timeout, value.c_str());
    return Setex(command);
}

bool RedisConnector::Setex(std::string key, const uint64 value, const int timeout) const
{
    const std::string command = strutil::format("SETEX %s %d %llu", key.c_str(), timeout, value);
    return Setex(command);
}

bool RedisConnector::Setex(std::string command) const
{
    redisReply* pRedisReply = static_cast<redisReply*>(redisCommand(_pRedisContext, command.c_str()));
    if (pRedisReply->type != REDIS_REPLY_STATUS || strncmp(pRedisReply->str, "OK", 2) != 0)
    {
        LOG_WARN("[SETEX] failed. command:" << command.c_str() << " pRedisReply->type:" << pRedisReply->type << " pRedisReply->str:" << pRedisReply->str);
        freeReplyObject(pRedisReply);
        return false;
    }

#if LOG_REDIS_OPEN
    LOG_REDIS(" @@ SETEX. key:" << command.c_str());
#endif

    freeReplyObject(pRedisReply);
    return true;
}

bool RedisConnector::SetnxExpire(std::string key, const int value, const int timeout) const
{
    if (_pRedisContext == nullptr)
    {
        LOG_WARN("RedisContext == nullptr. connect failed.");
        return false;
    }

    std::string command = strutil::format("SETNX %s %d", key.c_str(), value);
    redisReply* pRedisReply = static_cast<redisReply*>(redisCommand(_pRedisContext, command.c_str()));
    if (pRedisReply->type != REDIS_REPLY_INTEGER || pRedisReply->integer != 1)
    {
        // 没有设置成功，可能已经有值，可能redis挂掉，都认为已经在线
        LOG_WARN("[SETNX] failed 1. command:" << command.c_str() << " pRedisReply->type:" << pRedisReply->type << " pRedisReply->integer:" << pRedisReply->integer);
        freeReplyObject(pRedisReply);
        return false;
    }

#if LOG_REDIS_OPEN
    LOG_REDIS(" @@ SETNX. key:" << command.c_str());
#endif

    freeReplyObject(pRedisReply);

    // 成功之后，为这个值设一个时间限制，在这个时间内不能重复登录
    command = strutil::format("EXPIRE %s %d", key.c_str(), timeout);
    pRedisReply = static_cast<redisReply*>(redisCommand(_pRedisContext, command.c_str()));
    if (pRedisReply->type != REDIS_REPLY_INTEGER || pRedisReply->integer != 1)
    {
        // 没有设置成功，可能有异常，可能redis挂掉，都认为已经在线
        LOG_WARN("[EXPIRE] failed 2. command:" << command.c_str() << " pRedisReply->type:" << pRedisReply->type << " pRedisReply->integer:" << pRedisReply->integer);
        freeReplyObject(pRedisReply);
        return false;
    }

#if LOG_REDIS_OPEN
    LOG_REDIS(" @@ EXPIRE. key:" << command.c_str());
#endif

    freeReplyObject(pRedisReply);
    return true;
}

void RedisConnector::Delete(std::string key) const
{
    // del 返回值是被删除的数量
    std::string command = strutil::format("del %s", key.c_str());
    redisReply* pRedisReply = static_cast<redisReply*>(redisCommand(_pRedisContext, command.c_str()));
    if (pRedisReply->type != REDIS_REPLY_INTEGER)
    {
        LOG_ERROR("[DEL] execute command:" << command.c_str() << " pRedisReply->type:" << pRedisReply->type);
    }

    if (pRedisReply->integer == 0)
    {
        //LOG_COLOR(LogColorPurple, "[DEL] failed. command:" << command.c_str() << ". nothine is deleted. size = 0.");
    }
    else
    {
#if LOG_REDIS_OPEN
        LOG_REDIS(" @@ DEL. key:" << command.c_str());
#endif
    }

    freeReplyObject(pRedisReply);
}

int RedisConnector::GetInt(std::string key) const
{
    int nRs = 0;
    std::string command = strutil::format("get %s", key.c_str());
    redisReply* pRedisReply = static_cast<redisReply*>(redisCommand(_pRedisContext, command.c_str()));
    if (pRedisReply->type != REDIS_REPLY_INTEGER && pRedisReply->type != REDIS_REPLY_STRING)
    {
        //LOG_ERROR("[GET failed] execute command:" << command.c_str() << " pRedisReply->type:" << pRedisReply->type);
    }

    if (pRedisReply->type == REDIS_REPLY_INTEGER)
    {
        nRs = static_cast<int>(pRedisReply->integer);
    }
    else if (pRedisReply->type == REDIS_REPLY_STRING)
    {
        nRs = std::stoi(pRedisReply->str, nullptr, 10);
    }

    freeReplyObject(pRedisReply);
    return nRs;
}

std::string RedisConnector::GetString(std::string key) const
{
    std::string strRs = "";
    std::string command = strutil::format("get %s", key.c_str());
    redisReply* pRedisReply = static_cast<redisReply*>(redisCommand(_pRedisContext, command.c_str()));
    if (pRedisReply->type != REDIS_REPLY_STRING)
    {
        LOG_WARN("[GET failed] execute command:" << command.c_str() << " pRedisReply->type:" << pRedisReply->type);
    }
    else
    {
        strRs = pRedisReply->str;
    }

    freeReplyObject(pRedisReply);
    return strRs;
}

uint64 RedisConnector::GetUint64(std::string key) const
{
    uint64 rsValue = 0;
    std::string command = strutil::format("get %s", key.c_str());
    redisReply* pRedisReply = static_cast<redisReply*>(redisCommand(_pRedisContext, command.c_str()));
    if (pRedisReply->type != REDIS_REPLY_INTEGER && pRedisReply->type != REDIS_REPLY_STRING)
    {
        LOG_WARN("[GET failed] execute command:" << command.c_str() << " pRedisReply->type:" << pRedisReply->type);
    }

    if (pRedisReply->type == REDIS_REPLY_INTEGER)
    {
        rsValue = pRedisReply->integer;
    }
    else if (pRedisReply->type == REDIS_REPLY_STRING)
    {
        rsValue = std::stoll(pRedisReply->str, nullptr, 10);
    }

    freeReplyObject(pRedisReply);
    return rsValue;

}
