#include "socket_object.h"

SocketKey SocketKey::None = SocketKey(INVALID_SOCKET, NetworkType::None);

SocketKey::SocketKey(SOCKET socket, NetworkType netType)
{
    Socket = socket;
    NetType = netType;
}

void SocketKey::Clear()
{
    Socket = INVALID_SOCKET;
    NetType = NetworkType::None;
}

void SocketKey::CopyFrom(SocketKey* pSocketKey)
{
    Socket = pSocketKey->Socket;
    NetType = pSocketKey->NetType;
}

void TagKey::Clear()
{
    _tags.clear();
}

void TagKey::AddTag(TagType tagType, std::string value)
{
    const auto iter = _tags.find(tagType);
    if (iter == _tags.end())
    {
        _tags[tagType] = TagValue{ value, 0L };
    }
    else
    {
        _tags[tagType].KeyStr = value;
        _tags[tagType].KeyInt64 = 0;
    }
}

void TagKey::AddTag(TagType tagType, uint64 value)
{
    const auto iter = _tags.find(tagType);
    if (iter == _tags.end())
    {
        _tags[tagType] = TagValue{ "", value };
    }
    else
    {
        _tags[tagType].KeyStr = "";
        _tags[tagType].KeyInt64 = value;
    }
}

void TagKey::AddTag(TagType tagType, TagValue value)
{
    if (value.KeyInt64 != 0)
        AddTag(tagType, value.KeyInt64);
    else
        AddTag(tagType, value.KeyStr);
}

TagValue* TagKey::GetTagValue(TagType tagType)
{
    if (_tags.find(tagType) == _tags.end())
        return nullptr;

    return &(_tags[tagType]);
}

void TagKey::CopyFrom(TagKey* pNetIdentify)
{
    auto tags = pNetIdentify->GetTags();
    for (auto iter = tags->begin(); iter != tags->end(); ++iter)
    {
        AddTag(iter->first, iter->second);
    }
}

bool TagKey::CompareTags(TagKey* pIdentify)
{
    if (!CompareTags(this, pIdentify, TagType::Account))
        return false;

    if (!CompareTags(this, pIdentify, TagType::App))
        return false;

    return true;
}

bool TagKey::CompareTags(TagKey* pA, TagKey* pB, TagType tagtype)
{
    TagValue* pTagValueA = pA->GetTagValue(tagtype);
    TagValue* pTagValueB = pB->GetTagValue(tagtype);
    if (pTagValueA != nullptr && pTagValueB != nullptr)
    {
        if (IsTagTypeStr(tagtype))
            return pTagValueA->KeyStr == pTagValueB->KeyStr;
        else
            return pTagValueA->KeyInt64 == pTagValueB->KeyInt64;
    }

    return true;
}
//
//log4cplus::tostream& operator<<(log4cplus::tostream& os, TagKey* pTagKey)
//{
//    auto tags = pTagKey->GetTags();
//    for (auto iter = tags->begin(); iter != tags->end(); ++iter)
//    {
//        auto typeName = GetTagTypeName(iter->first);
//        os << " tag:" << typeName << " v:";
//
//        if (IsTagTypeStr(iter->first))
//            os << iter->second.KeyStr.c_str();
//        else
//            os << iter->second.KeyInt64;
//    }
//
//    return os;
//}

std::ostream& operator<<(std::ostream& os, TagKey* pTagKey)
{
    auto tags = pTagKey->GetTags();
    for (auto iter = tags->begin(); iter != tags->end(); ++iter)
    {
        auto typeName = GetTagTypeName(iter->first);
        os << " tag:" << typeName << " v:";

        if (IsTagTypeStr(iter->first))
            os << iter->second.KeyStr.c_str();
        else
            os << iter->second.KeyInt64;
    }

    return os;
}

//log4cplus::tostream& operator<<(log4cplus::tostream& os, NetIdentify* pIdentify)
//{
//    os << " socket:" << pIdentify->GetSocketKey()->Socket
//       << " networkType:" << GetNetworkTypeName(pIdentify->GetSocketKey()->NetType);
//
//    auto tags = pIdentify->GetTagKey();
//    os << tags;
//
//    return os;
//}

std::ostream& operator<<(std::ostream& os, NetIdentify* pIdentify)
{
    os << " socket:" << pIdentify->GetSocketKey()->Socket
        << " networkType:" << GetNetworkTypeName(pIdentify->GetSocketKey()->NetType);

    auto tags = pIdentify->GetTagKey();
    os << tags;

    return os;
}