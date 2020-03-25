#pragma once
#include "libserver/socket_object.h"
#include "libserver/common.h"
#include <string>

class LoginObj :public NetworkIdentify
{
public:
    LoginObj(NetworkIdentify* pIdentify, std::string account, std::string password);
    std::string GetAccount() const;
    std::string GetPassword() const;

private:
    std::string _password;
    std::string _account;
};

