#pragma once

#include "network/network_connector.h"
#include <random>
#include <thread>

class Client :public NetworkConnector
{
public:
	explicit Client(int msgCount, std::thread::id threadId);
	bool Connect(std::string ip, int port) override;
	void DataHandler();
	bool IsCompleted() const
	{
		return _isCompleted;
	}

private:
	std::string GenRandom();

private:
	int _msgCount;
	int _index{ 0 };

	bool _isCompleted{ false };

	std::string _lastMsg{ "" };

	std::default_random_engine* _pRandomEngine;
};

