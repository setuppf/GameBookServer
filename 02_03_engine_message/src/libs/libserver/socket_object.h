#pragma once

class ISocketObject
{
public:
	virtual ~ISocketObject()
	{
	}

	virtual SOCKET GetSocket() = 0;
};
