#pragma once
#include "libserver/system.h"
#include "libserver/entity.h"

class WorldComponentGather:public Entity<WorldComponentGather>, public IAwakeSystem<>
{
public:
	void Awake() override;
	void BackToPool() override;

private:
	void SyncWorldInfoToGather() const;
};

