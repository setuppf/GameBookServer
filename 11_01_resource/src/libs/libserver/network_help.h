#pragma once
#include "common.h"
#include "network_type.h"

class Packet;

class INetwork
{
public:
    virtual ~INetwork() = default;
    virtual void SendPacket(Packet*& pPacket) = 0;
};

class NetworkHelp
{
public:
    inline static bool IsTcp(NetworkType iType)
    {
        return iType == NetworkType::TcpConnector || iType == NetworkType::TcpListen;
    }

    inline static bool IsHttpMsg(int msgId)
    {
        if (msgId == Proto::MsgId::MI_HttpOuterRequest)
            return true;

        if (msgId == Proto::MsgId::MI_HttpOuterResponse)
            return true;

        if (msgId == Proto::MsgId::MI_HttpInnerResponse)
            return true;

        return false;
    }

    inline static bool IsError(int socketError)
    {
#if ENGINE_PLATFORM != PLATFORM_WIN32
        // EINPROGRESS : 当链接设置为非阻塞时，目标没有及时应答，正在执行中
        if (socketError == EINTR || socketError == EWOULDBLOCK || socketError == EAGAIN || socketError == EINPROGRESS)
            return false;
#else
        if (socketError == WSAEINTR || socketError == WSAEWOULDBLOCK || socketError == WSAEINPROGRESS)
            return false;
#endif

        return true;
    }
};
