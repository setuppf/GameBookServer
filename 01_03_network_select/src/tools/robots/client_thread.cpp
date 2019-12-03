
#include <iostream>

#include "client.h"
#include "client_thread.h"

ClientThread::ClientThread(int msgCount)
{
	_msgCount = msgCount;
}

void ClientThread::Update()
{
	if (_pClient == nullptr)
	{
		_pClient = new Client(_msgCount, _thread.get_id());
		if (!_pClient->Connect("127.0.0.1", 2233))
		{
			std::cout << "ClientThread Connect failed." << std::endl;
		}
	}
	else
	{
		_pClient->Update();
		_pClient->DataHandler();

		if (_pClient->IsCompleted())
		{
			Thread::Stop();
		}
	}

	std::this_thread::sleep_for(std::chrono::milliseconds(10));
}

void ClientThread::Dispose()
{
	if (_pClient != nullptr)
	{
		_pClient->Dispose();
		delete _pClient;
		_pClient = nullptr;
	}

	Thread::Dispose();
}
