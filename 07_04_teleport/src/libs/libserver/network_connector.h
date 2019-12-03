#pragma once

#include <utility>
#include "network.h"
#include "connect_obj.h"
#include "app_type.h"
#include "socket_object.h"

class Packet;

struct ConnectDetail :public SnObject, public IDisposable
{
public:
    ConnectDetail(TagType tagType, TagValue tagValue, std::string ip, int port)
    {
        TType = tagType;
        TValue = tagValue;
        Ip = std::move(ip);
        Port = port;
    };

    void Dispose() override { }

    std::string Ip{ "" };
    int Port{ 0 };

    TagType TType;
    TagValue TValue;
};

class NetworkConnector :
    public Network,
    public IAwakeSystem<int, int>
{
public:
    void Awake(int iType, int mixConnectAppType) override;

    virtual void Update();

    const char* GetTypeName() override;
    uint64 GetTypeHashCode() override;
    bool Connect(ConnectDetail* pDetail);

private:
    void HandleNetworkConnect(Packet* pPacket);  // ÇëÇóÒ»¸öconnector
    void CreateConnector(APP_TYPE appType, int appId, std::string ip, int port);

private:
    // waiting for connect to net
    CacheRefresh<ConnectDetail> _connecting;
};

