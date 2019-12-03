#pragma once

#include "network.h"

class NetworkListen :public Network, public IAwakeSystem<std::string, int>, public IAwakeSystem<int, int>
{
public:
    void Awake(std::string ip, int port);
    void Awake(int appType, int appId);
    void Awake(std::string ip, int port, NetworkType iType);

    void Update();
    const char* GetTypeName() override;

    static bool IsSingle() { return true; }

private:
    void HandleDisconnect(Packet* pPacket);

protected:
    virtual int Accept();

};
