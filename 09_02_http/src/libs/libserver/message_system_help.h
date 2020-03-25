#pragma once
#include "common.h"
#include "app_type.h"
#include "packet.h"

enum class NetworkType;
class INetwork;
struct http_message;

struct ParseUrlInfo
{
    std::string Host;
    int Port;
    std::string Mothed;
    std::string Params;
};

class MessageSystemHelp
{
public:
    static Packet* CreatePacket(Proto::MsgId msgId, NetworkIdentify* pIdentify);

    static void DispatchPacket(const Proto::MsgId msgId, NetworkIdentify* pIdentify);
    static void DispatchPacket(const Proto::MsgId msgId, google::protobuf::Message& proto, NetworkIdentify* pIdentify);

    static void SendPacket(const Proto::MsgId msgId, NetworkIdentify* pIdentify, google::protobuf::Message& proto);
    static void SendPacket(const Proto::MsgId msgId, google::protobuf::Message& proto, APP_TYPE appType, int appId = 0);

    // http
    // 发送请求
    static void SendHttpRequest(NetworkIdentify* pIdentify, std::string ip, int port, std::string method, std::map<std::string, std::string>* pParams);

    // 发送返回
    static void SendHttpResponse(NetworkIdentify* pIdentify, const char* content, int size);
    static void SendHttpResponse404(NetworkIdentify* pIdentify);

    // 分析数据
    static Packet* ParseHttp(NetworkIdentify* pIdentify, const char* s, unsigned int bodyLen, const bool isChunked, http_message* hm);
    static bool ParseUrl(const std::string& url, ParseUrlInfo& info);

protected:
    static void DispatchPacket(Packet* packet);
    static void SendPacket(Packet* pPacket);

    // http
    static void SendHttpResponseBase(NetworkIdentify* pIdentify, int status_code, const char* content, int size);
};

