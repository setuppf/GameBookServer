#pragma once
#include <memory>
#include "network.h"

class ConnectObj;
class Packet;

class NetworkConnector : public Network, virtual public IAwakeSystem<std::string, int>, public virtual IAwakeSystem<int, int>
{
public:
    void Awake(std::string ip, int port);
    void Awake(int appType, int appId);
    virtual void Update();
    bool IsConnected() const;
    const char* GetTypeName() override;

    static bool IsSingle() { return true; }

protected:
    bool Connect(std::string ip, int port);

private:
    void TryCreateConnectObj();

protected:
    std::string _ip{ "" };
    int _port{ 0 };
};

