#pragma once
#include "libserver/system.h"
#include "http_request.h"

class HttpRequestAccount : public HttpRequest, public IAwakeFromPoolSystem<std::string, std::string>
{
public:
    void Awake(std::string account, std::string password);

protected:
    void ProcessMsg(Json::Value value) override;

private:
    std::string _password;
};

