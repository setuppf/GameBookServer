#pragma once
#include "common.h"
#include "network_type.h"
#include <log4cplus/streams.h>
#include <ostream>

struct SocketKey
{
    SocketKey(SOCKET socket, NetworkType netType);

    void Clean();

    SOCKET Socket;
    NetworkType NetType;

    bool operator != (const SocketKey other)
    {
        if (Socket != other.Socket)
            return false;

        if (NetType != other.NetType)
            return false;

        return true;
    };

    bool operator == (const SocketKey other)
    {
        return (Socket == other.Socket) && (NetType == other.NetType);
    };

    static SocketKey None;
};

enum class ObjectKeyType
{
    None = Proto::NetworkObjectKeyType::ObjectKeyTypeNone,
    Account = Proto::NetworkObjectKeyType::ObjectKeyTypeAccount,
    App = Proto::NetworkObjectKeyType::ObjectKeyTypeApp,
};

inline const char* GetConnectKeyTypeName(const ObjectKeyType iType)
{
    if (iType == ObjectKeyType::Account)
        return "Account";
    else if (iType == ObjectKeyType::App)
        return "App";
    else
        return "None";
}

struct ObjectKeyValue
{
    uint64 KeyInt64{ 0 };
    std::string KeyStr{ "" };

    void Clean();

    bool operator != (const ObjectKeyValue other)
    {
        if (KeyInt64 != other.KeyInt64)
            return false;

        if (KeyStr != other.KeyStr)
            return false;

        return true;
    };

    bool operator == (const ObjectKeyValue other)
    {
        return (KeyInt64 == other.KeyInt64) && (KeyStr == other.KeyStr);
    };
};

struct ObjectKey
{
    ObjectKeyType KeyType{ ObjectKeyType::None };
    ObjectKeyValue KeyValue{ 0, "" };

    void ParseFromProto(Proto::NetworkObjectKey protoKey);
    void SerializeToProto(Proto::NetworkObjectKey* pProto) const;
    void Clean();

    bool operator != (const ObjectKey other)
    {
        if (KeyType != other.KeyType)
            return false;

        if (KeyValue != other.KeyValue)
            return false;

        return true;
    };

    bool operator == (const ObjectKey other)
    {
        return (KeyType == other.KeyType) && (KeyValue == other.KeyValue);
    };
};

struct NetworkIdentify
{
public:
    NetworkIdentify() = default;
    NetworkIdentify(SocketKey socketKey, ObjectKey objKey);

    virtual ~NetworkIdentify() = default;
    SocketKey GetSocketKey() const { return _socketKey; }
    ObjectKey GetObjectKey() const { return _objKey; }

protected:
    SocketKey _socketKey{ INVALID_SOCKET, NetworkType::None };
    ObjectKey _objKey{ ObjectKeyType::None , {0, ""} };
};

log4cplus::tostream& operator <<(log4cplus::tostream& os, const NetworkIdentify* pIdentify);
std::ostream& operator <<(std::ostream& os, NetworkIdentify* pIdentify);
