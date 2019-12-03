#pragma once

#include "network.h"

class NetworkListen :public Network, public IUpdateSystem, public IAwakeFromPoolSystem<std::string, int>
{
public:
    void AwakeFromPool(std::string ip, int port);
    void Update() override;
    const char* GetTypeName() override;

protected:
    virtual int Accept();
};
