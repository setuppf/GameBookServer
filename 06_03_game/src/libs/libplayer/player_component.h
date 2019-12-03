#pragma once

#include "libserver/common.h"

class PlayerComponent
{
public:
	virtual void ParserFromProto(const Proto::Player& proto) = 0;
	virtual void SerializeToProto(Proto::Player* pProto) = 0;
};

