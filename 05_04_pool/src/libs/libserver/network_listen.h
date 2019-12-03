#pragma once

#include "network.h"

class NetworkListen :public Network, public IAwakeSystem<std::string, int>
{
public:
    void Awake(std::string ip, int port);
    void Update();
    const char* GetTypeName() override;

private:
    void HandleDisconnect(Packet* pPacket);

protected:
    virtual int Accept();
};
