#pragma once
#include "libserver/common.h"

class Player;

class WorldProxyHelp
{
public:
	static void Teleport(Player* pPlayer, uint64 lastWorldSn, uint64 targetWorldSn);
};

