#pragma once
#include "libserver/entity.h"

#include "libplayer/player.h"

class PlayerManagerComponent:public Entity<PlayerManagerComponent>, public IAwakeFromPoolSystem<>
{
public:
	void Awake() override;
    void BackToPool() override;

	Player* AddPlayer(uint64 playerSn, uint64 worldSn, NetIdentify* pNetIdentify);
	Player* GetPlayerBySn(uint64 playerSn);
	void RemovePlayerBySn(uint64 playerSn);
	int OnlineSize() const;
	std::map<uint64, Player*>* GetAll();

private:
	std::map<uint64, Player*> _players;
};

