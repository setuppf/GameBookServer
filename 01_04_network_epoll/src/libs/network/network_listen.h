#pragma once

#include "network.h"

class NetworkListen :public Network
{
public:
	bool Listen(std::string ip, int port);
	void Update();

protected:
	virtual int Accept();
};
