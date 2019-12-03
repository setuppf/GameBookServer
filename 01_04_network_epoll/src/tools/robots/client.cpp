#include <thread>
#include <iostream>

#include "client.h"

#include "network/packet.h"
#include <sstream>
#include <random>

Client::Client(int msgCount, std::thread::id threadId)
{
	_msgCount = msgCount;

	// gen random seed 根据线程ID生成随机种子
	std::stringstream strStream;
	strStream << threadId;
	std::string idstr = strStream.str();
	std::seed_seq seed1(idstr.begin(), idstr.end());
	std::minstd_rand0 generator(seed1);

	_pRandomEngine = new std::default_random_engine(generator());
}

bool Client::Connect(std::string ip, int port)
{
	if (NetworkConnector::Connect(ip, port))
	{
		_lastMsg = "";
		return true;
	}

	return false;
}

void Client::DataHandler()
{
	if (_isCompleted)
		return;

	if (!IsConnected())
		return;

	if (_index < _msgCount)
	{
		// 发送数据
		if (_lastMsg.empty())
		{
			_lastMsg = GenRandom();
			//std::cout << "send msg.  << std::endl;
			//std::cout << "send msg. size:" << _lastMsg.length() << ". " << _lastMsg.c_str() << std::endl;

			Packet* pPacket = new Packet(1);
			pPacket->AddBuffer(_lastMsg.c_str(), _lastMsg.length());
			SendPacket(pPacket);
			delete pPacket;
		}
		else
		{
			if (HasRecvData())
			{
				Packet* pPacket = GetRecvPacket();
				if (pPacket != nullptr)
				{
					//const std::string msg(pPacket->GetBuffer(), pPacket->GetDataLength());
					//std::cout << "recv msg. size:" << pPacket->GetDataLength() << std::endl;

					//if (msg != _lastMsg)
					//	std::cout << " !!!!!!!!!!!!!!!!! error." << std::endl;

					_lastMsg = "";
					++_index;
					delete pPacket;
				}
			}
		}

	}
	else
	{
		_isCompleted = true;
	}
}

std::string Client::GenRandom()
{
	std::uniform_int_distribution<int> distribution{ 'a', 'z' };
	std::uniform_int_distribution<int> disLength(10, 20);
	int length = disLength(*_pRandomEngine);

	std::string rand_str(length, '\0');
	for (auto& one : rand_str)
	{
		one = distribution(*_pRandomEngine);
	}

	return rand_str;
}
