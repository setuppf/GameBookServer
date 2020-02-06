
#include <iostream>

#include "network/network.h"
#include <list>

int main(int argc, char* argv[])
{
    _sock_init();
    SOCKET socket = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (socket == INVALID_SOCKET)
    {
        std::cout << "::socket failed. err:" << _sock_err() << std::endl;
        return 1;
    }

    _sock_nonblock(socket);

    sockaddr_in addr;
    memset(&addr, 0, sizeof(sockaddr_in));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(2233);
    ::inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr.s_addr);

    if (::bind(socket, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) < 0)
    {
        std::cout << "::bind failed. err:" << _sock_err() << std::endl;
        return 1;
    }

    int backlog = GetListenBacklog();
    if (::listen(socket, backlog) < 0)
    {
        std::cout << "::listen failed." << _sock_err() << std::endl;
        return 1;
    }

    char buf[1024];
    memset(buf, 0, sizeof(buf));

    std::list<SOCKET> sockets;

    struct sockaddr socketClient;
    socklen_t socketLength = sizeof(socketClient);

    while (true)
    {
        SOCKET newSocket = ::accept(socket, &socketClient, &socketLength);
        if (newSocket != INVALID_SOCKET)
        {
            std::cout << "new connection.socket:" << newSocket << std::endl;
            _sock_nonblock(newSocket);
            sockets.push_back(newSocket);
        }

        auto iter = sockets.begin();
        while (iter != sockets.end())
        {
            SOCKET one = *iter;
            auto size = ::recv(one, buf, sizeof(buf), 0);
            if (size > 0)
            {
                std::cout << "::recv." << buf << " socket:" << one << std::endl;
                ::send(one, buf, size, 0);
                std::cout << "::send." << buf << " socket:" << one << std::endl << std::endl;

                _sock_close(one);
                iter = sockets.erase(iter);
            }
            else
            {
                ++iter;
            }
        }
    }

    return 0;
}

