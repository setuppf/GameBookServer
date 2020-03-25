#pragma once
#include "libserver/entity.h"
#include "libserver/system.h"

class Packet;

class WorldOperatorComponent : public Entity<WorldOperatorComponent>, public IAwakeSystem<>
{
public:
	void Awake() override;
    void BackToPool() override;

private:
	void HandleCreateWorld(Packet* pPacket);
};
