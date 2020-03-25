#pragma once
#include "common.h"

#include <log4cplus/streams.h>
#include <ostream>

#include "network_type.h"

struct SocketKey
{
    SocketKey(SOCKET socket, NetworkType netType);

    void Clear();
    void CopyFrom(SocketKey* pSocketKey);

    SOCKET Socket;
    NetworkType NetType;

    bool operator != (const SocketKey other) const
    {
        if (Socket != other.Socket)
            return false;

        if (NetType != other.NetType)
            return false;

        return true;
    };

    bool operator == (const SocketKey other) const
    {
        return (Socket == other.Socket) && (NetType == other.NetType);
    };

    static SocketKey None;
};

enum class TagType
{
    None = Proto::TagType::TagTypeNone,
    Account = Proto::TagType::TagTypeAccount,
    App = Proto::TagType::TagTypeApp,
    Entity = Proto::TagType::TagTypeEntity,
    ToWorld = Proto::TagType::TagTypeToWorld,
    Player = Proto::TagType::TagTypePlayer,
};

inline bool IsTagTypeStr(const TagType iType)
{
    return iType == TagType::Account;
}

inline const char* GetTagTypeName(const TagType iType)
{
    if (iType == TagType::Account)
        return "Account";
    else if (iType == TagType::App)
        return "App";
    else if (iType == TagType::Entity)
        return "world";
    else if (iType == TagType::Player)
        return "player";
    else
        return "None";
}

struct TagValue
{
    std::string KeyStr{ "" };
    uint64 KeyInt64{ 0 };

    bool operator != (const TagValue& other) const
    {
        return (KeyStr != other.KeyStr) || (KeyInt64 != other.KeyInt64);
    };

    bool operator == (const TagValue& other) const
    {
        return (KeyStr == other.KeyStr) && (KeyInt64 == other.KeyInt64);
    };
};

struct TagKey
{
public:
    void Clear();

    std::map<TagType, TagValue>* GetTags() { return &_tags; }

    void AddTag(TagType tagType, std::string value);
    void AddTag(TagType tagType, uint64 value);
    void AddTag(TagType tagType, TagValue value);

    TagValue* GetTagValue(TagType tagType);
    void CopyFrom(TagKey* pNetIdentify);
    bool CompareTags(TagKey* pIdentify);

protected:
    static bool CompareTags(TagKey* pA, TagKey* pB, TagType tagtype);

protected:
    std::map<TagType, TagValue> _tags;
};

struct NetIdentify
{
public:
    NetIdentify() = default;
    ~NetIdentify()
    {
        _socketKey.Clear();
        _tagKey.Clear();
    }

    SocketKey* GetSocketKey() { return &_socketKey; }
    TagKey* GetTagKey() { return &_tagKey; }

protected:
    SocketKey _socketKey{ INVALID_SOCKET, NetworkType::None };
    TagKey _tagKey;
};

std::ostream& operator <<(std::ostream& os, TagKey* pTagKey);
std::ostream& operator <<(std::ostream& os, NetIdentify* pIdentify);

#if ENGINE_PLATFORM == PLATFORM_WIN32
log4cplus::tostream& operator <<(log4cplus::tostream& os, TagKey* pTagKey);
log4cplus::tostream& operator <<(log4cplus::tostream& os, NetIdentify* pIdentify);
#endif
