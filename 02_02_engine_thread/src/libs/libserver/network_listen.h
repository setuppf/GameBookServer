#pragma once

#include "network.h"

class NetworkListen :public Network
{
public:
	bool Init( ) override;
	void RegisterMsgFunction( ) override;
	bool Listen(std::string ip, int port);
	void Update( ) override;

protected:
	virtual int Accept();
};
