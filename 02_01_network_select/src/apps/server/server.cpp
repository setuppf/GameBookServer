
#include <iostream>

#include "server.h"
#include "network/packet.h"
#include "network/connect_obj.h"

bool Server::DataHandler()
{
	for (auto iter = _connects.begin(); iter != _connects.end(); ++iter)
	{
		ConnectObj* pConnectObj = iter->second;
		HandlerOne(pConnectObj);
	}

	if (_isShow)
	{
		_isShow = false;
		std::cout << "accept:" << _acceptCount << "\trecv count:" << _recvMsgCount << "\tsend count:" << _sendMsgCount << std::endl;
	}

	return true;
}

void Server::HandlerOne(ConnectObj* pConnectObj)
{
	// 收到客户端的消息，马上原样发送出去
	while (pConnectObj->HasRecvData())
	{
		Packet* pPacket = pConnectObj->GetRecvPacket();
		if (pPacket == nullptr)
		{
			// 数据不全，下帧再检查
			break;
		}

		std::string msg(pPacket->GetBuffer(), pPacket->GetDataLength());
		std::cout << "recv size:" << msg.length() << " msg:" << msg.c_str() << std::endl;
		pConnectObj->SendPacket(pPacket);

		++_recvMsgCount;
		++_sendMsgCount;
		_isShow = true;
	}
}

int Server::Accept()
{
	int rs = NetworkListen::Accept();
	_acceptCount += rs;
	_isShow = true;
	return rs;
}
