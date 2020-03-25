#pragma once
#include "libserver/redis_connector.h"

class Packet;

class RedisGame : public RedisConnector
{
private:
	void RegisterMsgFunction() override;

	void HandlePlayerSyncOnlineToRedis(Packet* pPacket);
	void HandlePlayerDeleteOnlineToRedis(Packet* pPacket);

	void HandleGameTokenToRedis(Packet* pPacket);
};
