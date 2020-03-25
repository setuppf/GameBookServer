
#include "network/network_buffer.h"
#include <string>
#include <cstring>
#include "network/packet.h"
#include <algorithm>
#include <iostream>

void SendData(SendNetworkBuffer* pSendbuff, RecvNetworkBuffer* pRecvbuff)
{
	char* pSendBuffer = nullptr;
	char* pRecvBuffer = nullptr;

	// 将发送区的数据，COPY到接收区中去，模拟数据传输
	while (true)
	{
		const int needSendSize = pSendbuff->GetBuffer(pSendBuffer);
		if (needSendSize <= 0)
			break;

		if (pRecvbuff->GetEmptySize() < (sizeof(PacketHead) + sizeof(TotalSizeType)))
		{
			pRecvbuff->ReAllocBuffer();
		}

		int emptySize = pRecvbuff->GetBuffer(pRecvBuffer);
		const int mixSize = std::min(needSendSize, emptySize);
		::memcpy(pRecvBuffer, pSendBuffer, mixSize);

		pSendbuff->RemoveDate(mixSize);
		pRecvbuff->FillDate(mixSize);
	}
}

void RecvData(RecvNetworkBuffer* pRecvbuff)
{
	// 读出数据
	while (pRecvbuff->HasData())
	{
		const auto pPacket = pRecvbuff->GetPacket();
		std::string msg(pPacket->GetBuffer(), pPacket->GetDataLength());
		std::cout << "recv msg:" << msg.c_str() << std::endl;
	}
}

void TestSendBuffer1()
{
	// 测试缓冲区满的情况
	SendNetworkBuffer sendbuff(DEFAULT_SEND_BUFFER_SIZE);
	RecvNetworkBuffer recvbuff(DEFAULT_RECV_BUFFER_SIZE);

	std::string msg = "123456"; // 加上totol 2 + head 2 一共是10位，正好一个缓冲区

	// 10个byte
	Packet packet(1);
	packet.AddBuffer(msg.c_str(), msg.length());
	sendbuff.AddPacket(&packet);

	// 10个byte
	msg = "654321";
	packet.Dispose();
	packet.AddBuffer(msg.c_str(), msg.length());
	sendbuff.AddPacket(&packet);
 
	SendData(&sendbuff, &recvbuff);
	RecvData(&recvbuff);

	std::cout << "send buffer. begin index:" << sendbuff.GetBeginIndex() << " end index:" << sendbuff.GetEndIndex() << " total size:" << sendbuff.GetTotalSize() << std::endl;
	std::cout << "recv buffer. begin index:" << recvbuff.GetBeginIndex() << " end index:" << recvbuff.GetEndIndex() << " total size:" << recvbuff.GetTotalSize() << std::endl;

	// 10个byte
	msg = "111223";
	packet.Dispose();
	packet.AddBuffer(msg.c_str(), msg.length());
	sendbuff.AddPacket(&packet);

	SendData(&sendbuff, &recvbuff);
	RecvData(&recvbuff);

	std::cout << "send buffer. begin index:" << sendbuff.GetBeginIndex() << " end index:" << sendbuff.GetEndIndex() << " total size:" << sendbuff.GetTotalSize() << std::endl;
	std::cout << "recv buffer. begin index:" << recvbuff.GetBeginIndex() << " end index:" << recvbuff.GetEndIndex() << " total size:" << recvbuff.GetTotalSize() << std::endl;

	std::cout << "************** case over *******************" << std::endl;
}

void TestSendBuffer2()
{
	// 测试 totalsize 被分成尾部和头部的情况
	SendNetworkBuffer sendbuff(DEFAULT_SEND_BUFFER_SIZE * 2);
	RecvNetworkBuffer recvbuff(DEFAULT_RECV_BUFFER_SIZE * 2);

	std::string msg = "12345"; // 加上totol 2 + head 2

	// 9个byte
	Packet packet(1);
	packet.AddBuffer(msg.c_str(), msg.length());
	sendbuff.AddPacket(&packet);

	// 10个byte
	msg = "123456";
	packet.Dispose();
	packet.AddBuffer(msg.c_str(), msg.length());
	sendbuff.AddPacket(&packet);

	SendData(&sendbuff, &recvbuff);
	RecvData(&recvbuff);

	std::cout << "send buffer. begin index:" << sendbuff.GetBeginIndex() << " end index:" << sendbuff.GetEndIndex() << " total size:" << sendbuff.GetTotalSize() << std::endl;
	std::cout << "recv buffer. begin index:" << recvbuff.GetBeginIndex() << " end index:" << recvbuff.GetEndIndex() << " total size:" << recvbuff.GetTotalSize() << std::endl;

	// 10个byte
	msg = "654321";
	packet.Dispose();
	packet.AddBuffer(msg.c_str(), msg.length());
	sendbuff.AddPacket(&packet);

	SendData(&sendbuff, &recvbuff);
	RecvData(&recvbuff);

	std::cout << "send buffer. begin index:" << sendbuff.GetBeginIndex() << " end index:" << sendbuff.GetEndIndex() << " total size:" << sendbuff.GetTotalSize() << std::endl;
	std::cout << "recv buffer. begin index:" << recvbuff.GetBeginIndex() << " end index:" << recvbuff.GetEndIndex() << " total size:" << recvbuff.GetTotalSize() << std::endl;

	std::cout << "************** case over *******************" << std::endl;
}

void TestSendBuffer3()
{
	// 测试 Head 被分成尾部和头部的情况
	SendNetworkBuffer sendbuff(DEFAULT_SEND_BUFFER_SIZE * 2);
	RecvNetworkBuffer recvbuff(DEFAULT_RECV_BUFFER_SIZE * 2);

	std::string msg = "123"; // 加上totol 2 + head 2

	// 7个byte
	Packet packet(1);
	packet.AddBuffer(msg.c_str(), msg.length());
	sendbuff.AddPacket(&packet);

	// 10个byte
	msg = "654321";
	packet.Dispose();
	packet.AddBuffer(msg.c_str(), msg.length());
	sendbuff.AddPacket(&packet);

	SendData(&sendbuff, &recvbuff);
	RecvData(&recvbuff);

	std::cout << "send buffer. begin index:" << sendbuff.GetBeginIndex() << " end index:" << sendbuff.GetEndIndex() << " total size:" << sendbuff.GetTotalSize() << std::endl;
	std::cout << "recv buffer. begin index:" << recvbuff.GetBeginIndex() << " end index:" << recvbuff.GetEndIndex() << " total size:" << recvbuff.GetTotalSize() << std::endl;

	// 10个byte
	msg = "123456";
	packet.Dispose();
	packet.AddBuffer(msg.c_str(), msg.length());
	sendbuff.AddPacket(&packet);

	SendData(&sendbuff, &recvbuff);
	RecvData(&recvbuff);

	std::cout << "send buffer. begin index:" << sendbuff.GetBeginIndex() << " end index:" << sendbuff.GetEndIndex() << " total size:" << sendbuff.GetTotalSize() << std::endl;
	std::cout << "recv buffer. begin index:" << recvbuff.GetBeginIndex() << " end index:" << recvbuff.GetEndIndex() << " total size:" << recvbuff.GetTotalSize() << std::endl;

	std::cout << "************** case over *******************" << std::endl;
}

int main(int argc, char *argv[])
{
	TestSendBuffer1();
	TestSendBuffer2();
	TestSendBuffer3();
	return 0;
}

