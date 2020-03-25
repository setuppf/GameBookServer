#include "redis_login.h"
#include "libserver/protobuf/proto_id.pb.h"
#include "libserver/redis_constants.h"
#include "libserver/log4_help.h"
#include "libserver/thread_mgr.h"

#include "libserver/message_system_help.h"
#include "libserver/global.h"
#include "libserver/message_system.h"

void RedisLogin::RegisterMsgFunction()
{
    auto pMsgSystem = GetSystemManager()->GetMessageSystem();

    pMsgSystem->RegisterFunction(this, Proto::MsgId::MI_AccountSyncOnlineToRedis, BindFunP1(this, &RedisLogin::HandleAccountSyncOnlineToRedis));
    pMsgSystem->RegisterFunction(this, Proto::MsgId::MI_AccountDeleteOnlineToRedis, BindFunP1(this, &RedisLogin::HandleAccountDeleteOnlineToRedis));

    pMsgSystem->RegisterFunction(this, Proto::MsgId::MI_LoginTokenToRedis, BindFunP1(this, &RedisLogin::HandleLoginTokenToRedis));
    pMsgSystem->RegisterFunction(this, Proto::MsgId::MI_AccountQueryOnlineToRedis, BindFunP1(this, &RedisLogin::HandleAccountQueryOnline));
}


void RedisLogin::HandleAccountSyncOnlineToRedis(Packet* pPacket)
{
    auto proto = pPacket->ParseToProto<Proto::AccountSyncOnlineToRedis>();

    const std::string key = RedisKeyAccountOnlineLogin + proto.account();
    const std::string value = std::to_string(Global::GetInstance()->GetCurAppId());
    Setex(key, value, RedisKeyAccountOnlineLoginTimeout);
}

void RedisLogin::HandleAccountDeleteOnlineToRedis(Packet* pPacket)
{
    auto proto = pPacket->ParseToProto<Proto::AccountDeleteOnlineToRedis>();
    const std::string key = RedisKeyAccountOnlineLogin + proto.account();

    Delete(key);
}

void RedisLogin::HandleLoginTokenToRedis(Packet* pPacket)
{
    auto protoToken = pPacket->ParseToProto<Proto::LoginTokenToRedis>();
    auto account = protoToken.account();
    auto playerSn = protoToken.player_sn();

    auto token = Global::GetInstance()->GenerateUUID();

    Proto::TokenInfo protoInfo;
    protoInfo.set_token(token);
    protoInfo.set_player_sn(playerSn);

    std::string tokenString;
    protoInfo.SerializeToString(&tokenString);

    const std::string key = RedisKeyAccountTokey + account;
    const int timeoue = RedisKeyAccountTokeyTimeout;

    if (!Setex(key, tokenString, timeoue))
    {
        token = "";
        LOG_ERROR("account:" << account.c_str() << ". failed to set token.");
    }
    else
    {
#if LOG_REDIS_OPEN
        LOG_REDIS("account:" << account.c_str() << ". gen token:" << token.c_str());
#endif
    }

    Proto::LoginTokenToRedisRs protoRs;
    protoRs.set_account(account.c_str());
    protoRs.set_token(token.c_str());
    MessageSystemHelp::DispatchPacket(Proto::MsgId::MI_LoginTokenToRedisRs, protoRs, nullptr);
}

void RedisLogin::HandleAccountQueryOnline(Packet* pPacket)
{
    auto proto = pPacket->ParseToProto<Proto::AccountQueryOnlineToRedis>();
    auto account = proto.account();
    Proto::AccountQueryOnlineToRedisRs protoRs;
    protoRs.set_account(account.c_str());
    protoRs.set_return_code(Proto::AccountQueryOnlineToRedisRs::SOTR_Offline);

    // 是否正在登录
    if (!SetnxExpire(RedisKeyAccountOnlineLogin + proto.account(), Global::GetInstance()->GetCurAppId(), RedisKeyAccountOnlineLoginTimeout))
        protoRs.set_return_code(Proto::AccountQueryOnlineToRedisRs::SOTR_Online);

    // 是否 Game 在线
    if (GetInt(RedisKeyAccountOnlineGame + proto.account()) != 0)
        protoRs.set_return_code(Proto::AccountQueryOnlineToRedisRs::SOTR_Online);


    MessageSystemHelp::DispatchPacket(Proto::MsgId::MI_AccountQueryOnlineToRedisRs, protoRs, nullptr);
}
