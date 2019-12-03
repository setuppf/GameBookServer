#pragma once
#include <memory>
#include "network.h"

class ConnectObj;
class Packet;

class NetworkConnector : public Network, public IUpdateSystem, public IAwakeFromPoolSystem <std::string, int>
{
public:
    void AwakeFromPool(std::string ip, int port);
    void Update() override;
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

