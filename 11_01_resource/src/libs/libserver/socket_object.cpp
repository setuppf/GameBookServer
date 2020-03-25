#include "socket_object.h"

SocketKey SocketKey::None = SocketKey(INVALID_SOCKET, NetworkType::None);

SocketKey::SocketKey(SOCKET socket, NetworkType netType)
{
    Socket = socket;
    NetType = netType;
}

void SocketKey::Clean()
{
    Socket = INVALID_SOCKET;
    NetType = NetworkType::None;
}

void ObjectKeyValue::Clean()
{
    KeyInt64 = 0;
    KeyStr = "";
}

void ObjectKey::ParseFromProto(Proto::NetworkObjectKey protoKey)
{
    if (protoKey.key_type() == Proto::NetworkObjectKeyType::ObjectKeyTypeApp)
    {
        KeyType = ObjectKeyType::App;
        KeyValue.KeyInt64 = protoKey.key_value().key_int64();
        KeyValue.KeyStr = "";
    }
    else if (protoKey.key_type() == Proto::NetworkObjectKeyType::ObjectKeyTypeAccount)
    {
        KeyType = ObjectKeyType::Account;
        KeyValue.KeyInt64 = 0;
        KeyValue.KeyStr = protoKey.key_value().key_str();
    }
    else
    {
        KeyType = ObjectKeyType::None;
        KeyValue.KeyInt64 = 0;
        KeyValue.KeyStr = "";
    }
}

void ObjectKey::SerializeToProto(Proto::NetworkObjectKey* pProto) const
{
    pProto->set_key_type((Proto::NetworkObjectKeyType)KeyType);
    auto pKeyValue = pProto->mutable_key_value();
    pKeyValue->set_key_int64(KeyValue.KeyInt64);
    pKeyValue->set_key_str(KeyValue.KeyStr.c_str());
}

void ObjectKey::Clean()
{
    KeyType = ObjectKeyType::None;
    KeyValue.Clean();
}

NetworkIdentify::NetworkIdentify(SocketKey socketKey, ObjectKey objKey)
{
    _socketKey = socketKey;
    _objKey = objKey;
}

log4cplus::tostream& operator<<(log4cplus::tostream& os, const NetworkIdentify* pIdentify)
{
    os << " socket:" << pIdentify->GetSocketKey().Socket << " networkType:" << GetNetworkTypeName(pIdentify->GetSocketKey().NetType) <<
        " connect type:" << GetConnectKeyTypeName(pIdentify->GetObjectKey().KeyType) << " value:";

    if (pIdentify->GetObjectKey().KeyType == ObjectKeyType::Account)
        os << pIdentify->GetObjectKey().KeyValue.KeyStr.c_str();
    else
        os << pIdentify->GetObjectKey().KeyValue.KeyInt64;

    return os;
}

std::ostream& operator<<(std::ostream& os, NetworkIdentify* pIdentify)
{
    os << " socket:" << pIdentify->GetSocketKey().Socket
        << " networkType:" << GetNetworkTypeName(pIdentify->GetSocketKey().NetType)
        << " connect type:" << GetConnectKeyTypeName(pIdentify->GetObjectKey().KeyType) << " value:";

    if (pIdentify->GetObjectKey().KeyType == ObjectKeyType::Account)
        os << pIdentify->GetObjectKey().KeyValue.KeyStr.c_str();
    else
        os << pIdentify->GetObjectKey().KeyValue.KeyInt64;

    return os;
}