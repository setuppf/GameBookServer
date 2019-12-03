#pragma once
#include <memory>
#include "network.h"

class ConnectObj;
class Packet;

class NetworkConnector : public Network, public IAwakeFromPoolSystem <std::string, int>, public IAwakeFromPoolSystem<int, int>
{
public:
    void AwakeFromPool(std::string ip, int port);
    void AwakeFromPool(int appType, int appId);
    virtual void Update();
    bool IsConnected() const;
	const char* GetTypeName() override;

protected:
	bool Connect(std::string ip, int port);

private:
    void TryCreateConnectObj();

protected:
    std::string _ip{ "" };
    int _port{ 0 };
};

