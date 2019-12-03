#pragma once
#include "common.h"

class ISocketObject
{
public:
	virtual ~ISocketObject()
	{
	}

	virtual SOCKET GetSocket() = 0;
};
