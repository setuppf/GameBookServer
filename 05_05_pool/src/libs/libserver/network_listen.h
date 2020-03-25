#pragma once

#include "network.h"

class NetworkListen :public Network
{
public:
    bool Init() override;
    bool Listen(std::string ip, int port);
    void Update() override;

protected:
    virtual int Accept();
};
