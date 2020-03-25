#pragma once

#include <map>
#include "common.h"
#include "entity.h"
#include "socket_object.h"
#include "cache_swap.h"
#include "message_system.h"
#include "network_interface.h"

#if ENGINE_PLATFORM != PLATFORM_WIN32
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h> 

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/tcp.h>

#ifdef EPOLL
#include <sys/epoll.h>
#endif

#define _sock_init( )
#define _sock_nonblock( sockfd ) { int flags = fcntl(sockfd, F_GETFL, 0); fcntl(sockfd, F_SETFL, flags | O_NONBLOCK); }
#define _sock_exit( )
#define _sock_err( )	errno
#define _sock_close( sockfd ) ::shutdown( sockfd, SHUT_RDWR ) 
#define _sock_is_blocked()	(errno == EAGAIN || errno == 0)

#else

#define _sock_init( )	{ WSADATA wsaData; WSAStartup( MAKEWORD(2, 2), &wsaData ); }
#define _sock_nonblock( sockfd )	{ unsigned long param = 1; ioctlsocket(sockfd, FIONBIO, (unsigned long *)&param); }
#define _sock_exit( )	{ WSACleanup(); }
#define _sock_err( )	WSAGetLastError()
#define _sock_close( sockfd ) ::closesocket( sockfd )
#define _sock_is_blocked()	(WSAGetLastError() == WSAEWOULDBLOCK)

#endif

class ConnectObj;
class Packet;

class Network : public Entity<Network>, public IMessageSystem, public INetwork
{
public:
    void BackToPool() override;
    void RegisterMsgFunction() override;
    SOCKET GetSocket() override { return _masterSocket; }
    void SendPacket(Packet*& pPacket) override;
    bool IsBroadcast() { return _isBroadcast; }

protected:
    static void SetSocketOpt(SOCKET socket);
    static SOCKET CreateSocket();
    void CreateConnectObj(SOCKET socket);
    void Clean();

#ifdef EPOLL
    void InitEpoll();
    void Epoll();
    void AddEvent(int epollfd, int fd, int flag);
    void ModifyEvent(int epollfd, int fd, int flag);
    void DeleteEvent(int epollfd, int fd);
#else
    void Select();
#endif

    void OnNetworkUpdate();

private:
    void HandleDisconnect(Packet* pPacket);

protected:
    SOCKET _masterSocket{ INVALID_SOCKET };
    std::map<SOCKET, ConnectObj*> _connects;

#ifdef EPOLL
#define MAX_CLIENT  5120
#define MAX_EVENT   5120
    struct epoll_event _events[MAX_EVENT];
    int _epfd;
    int _mainSocketEventIndex{ -1 };
#else
    fd_set readfds, writefds, exceptfds;
#endif

    // 发送协议
    std::mutex _sendMsgMutex;
    CacheSwap<Packet> _sendMsgList;

    // 收到的协议是否需要广播到全线程
    bool _isBroadcast{ true };
};
