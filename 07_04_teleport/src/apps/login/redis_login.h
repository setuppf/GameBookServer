#pragma once

#include "libserver/redis_connector.h"

class Packet;

class RedisLogin : public RedisConnector
{
private:
    void RegisterMsgFunction() override;

    void HandleLoginTokenToRedis(Packet* pPacket);
    void HandleAccountQueryOnline(Packet* pPacket);

    void HandleAccountSyncOnlineToRedis(Packet* pPacket);
    void HandleAccountDeleteOnlineToRedis(Packet* pPacket);
};

