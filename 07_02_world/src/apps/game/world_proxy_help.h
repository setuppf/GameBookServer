#pragma once
#include "libserver/common.h"

class Player;

class WorldProxyHelp
{
public:
	static void Teleport(Player* pPlayer, uint64 oriWorldSn, uint64 targetWorldSn);

};

