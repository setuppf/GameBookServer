#pragma once
#include "network/thread.h"

class Client;

class ClientThread : public Thread
{
public:
	explicit ClientThread(int msgCount);
	void Update() override;
	void Dispose() override;

private:
	Client * _pClient{ nullptr };
	int _msgCount;
};

