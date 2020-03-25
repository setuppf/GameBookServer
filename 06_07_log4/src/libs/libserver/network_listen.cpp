#include <iostream>

#include "common.h"
#include "network_listen.h"
#include "connect_obj.h"
#include "thread_mgr.h"
#include "network_locator.h"
#include "log4_help.h"

void NetworkListen::AwakeFromPool(std::string ip, int port)
{
    auto pNetworkLocator = ThreadMgr::GetInstance()->GetComponent<NetworkLocator>();
    pNetworkLocator->AddListenLocator(this, NetworkTcpListen);

    _masterSocket = CreateSocket();
    if (_masterSocket == INVALID_SOCKET)
        return;

    sockaddr_in addr;
    memset(&addr, 0, sizeof(sockaddr_in));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    ::inet_pton(AF_INET, ip.c_str(), &addr.sin_addr.s_addr);

    if (::bind(_masterSocket, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) < 0)
    {
        std::cout << "::bind failed. err:" << _sock_err() << std::endl;
        return;
    }

    if (::listen(_masterSocket, SOMAXCONN) < 0)
    {
        std::cout << "::listen failed." << _sock_err() << std::endl;
        return;
    }

#ifdef EPOLL
    LOG_INFO("epoll model. listen " << ip.c_str() << ":" << port);
    InitEpoll();
#else
    LOG_INFO("select model. listen " << ip.c_str() << ":" << port);
#endif

    return;
}

int NetworkListen::Accept()
{
    struct sockaddr socketClient;
    socklen_t socketLength = sizeof(socketClient);

    int rs = 0;
    while (true)
    {
        const SOCKET socket = ::accept(_masterSocket, &socketClient, &socketLength);
        if (socket == INVALID_SOCKET)
            return rs;

        //std::cout << "accept socket:" << socket << std::endl;
        SetSocketOpt(socket);
        CreateConnectObj(socket);

        ++rs;
    }
}

const char* NetworkListen::GetTypeName()
{
    return typeid(NetworkListen).name();
}

#ifndef EPOLL

void NetworkListen::Update()
{
    Select();

    if (FD_ISSET(_masterSocket, &readfds))
    {
        Accept();
    }

    Network::OnNetworkUpdate();
}

#else

void NetworkListen::Update()
{
    Epoll();

    if (_mainSocketEventIndex >= 0)
    {
        Accept();
    }

    Network::OnNetworkUpdate();
}

#endif

