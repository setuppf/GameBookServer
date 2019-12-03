#pragma once
#include "http_request.h"

class HttpRequestAccount : public HttpRequest
{
public:
    HttpRequestAccount(std::string account, std::string password);

protected:
    void ProcessMsg(Json::Value value) override;

private:
    std::string _password;
};

