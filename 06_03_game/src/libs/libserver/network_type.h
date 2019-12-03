#pragma once

enum class NetworkType
{
    None = 0,
    TcpListen = 1 << 0,
    TcpConnector = 1 << 1,

    HttpListen = 1 << 2,
    HttpConnector = 1 << 3,
};

inline const char* GetNetworkTypeName(const NetworkType appType)
{
    if (appType == NetworkType::TcpListen)
        return "TcpListen";
    else if (appType == NetworkType::TcpConnector)
        return "TcpConnector";
    else if (appType == NetworkType::HttpListen)
        return "HttpListen";
    else
        return "HttpConnector";
}
