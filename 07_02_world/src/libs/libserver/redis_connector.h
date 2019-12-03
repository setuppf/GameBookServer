#pragma once

#include "entity.h"
#include "system.h"

#include <hiredis/hiredis.h>

class RedisConnector :public Entity<RedisConnector>, public IAwakeSystem<>
{
public:
	void Awake() override;	
	void BackToPool() override;

	bool Connect();
	bool Disconnect();

protected:
	virtual void RegisterMsgFunction() = 0;

	bool Ping() const;
	void CheckPing();

	bool Setex(std::string key, std::string value, int timeout) const;
	bool Setex(std::string key, uint64 value, int timeout) const;
	bool SetnxExpire(std::string key, int value, int timeout) const;
	void Delete(std::string key) const;

	int GetInt(std::string key) const;
	std::string GetString(std::string key) const;
	uint64 GetUint64(std::string key) const;

private:
	bool Setex(std::string command) const;

protected:
	redisContext * _pRedisContext{ nullptr };

	std::string _ip{ "127.0.0.1" };
	int _port{ 8080 };
};



