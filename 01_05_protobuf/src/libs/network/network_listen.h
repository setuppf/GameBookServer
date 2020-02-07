#pragma once

#include "network.h"

class NetworkListen :public Network
{
public:
	bool Init( );
	void RegisterMsgFuntion( );
	bool Listen(std::string ip, int port);
	void Update();

protected:
	virtual int Accept();
};
