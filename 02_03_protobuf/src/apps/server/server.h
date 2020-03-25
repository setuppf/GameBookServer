#pragma once
#include "network/network_listen.h"

class Server : public NetworkListen
{
public:
	bool DataHandler();

protected:
	void HandlerOne(ConnectObj* pConnectObj);
	int Accept() override;

protected:
	int _acceptCount{ 0 };
	int _sendMsgCount{ 0 };
	int _recvMsgCount{ 0 };

	bool _isShow{ true };
};

