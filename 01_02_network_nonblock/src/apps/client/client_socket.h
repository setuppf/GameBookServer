#pragma once
#include <thread>

class ClientSocket
{
public:
	ClientSocket(int index);
	void SendMsg();
	bool IsRun() const;
	void Stop();

private:
	bool _isRun{ true };
	int _curIndex;
	std::thread _thread;
};

