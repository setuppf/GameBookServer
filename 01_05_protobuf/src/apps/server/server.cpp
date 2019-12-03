
#include <iostream>

#include "server.h"
#include "network/common.h"
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
		//std::cout << "accept:" << _acceptCount << "\trecv msg:" << _recvMsgCount << "\tsend msg:" << _sendMsgCount << std::endl;
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

		Proto::TestMsg protoMsg = pPacket->ParseToProto<Proto::TestMsg>();
		std::cout << "revc msg. msg id:" << pPacket->GetMsgId() << " msg:" << protoMsg.msg() << " index:" << protoMsg.index() << std::endl;

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
