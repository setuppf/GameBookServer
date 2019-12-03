#include "http_request_account.h"
#include <iostream>

#include "libserver/common.h"
#include "libserver/thread_mgr.h"
#include "libserver/packet.h"
#include "libserver/message_system.h"
#include "libserver/yaml.h"

void HttpRequestAccount::AwakeFromPool(std::string account, std::string password)
{
    _account = account;
    _password = password;
    _curlRs = CRS_None;
    _method = HttpResquestMethod::HRM_Post;

    auto pYaml = Yaml::GetInstance();
    const auto pLoginConfig = dynamic_cast<LoginConfig*>(pYaml->GetConfig(APP_LOGIN));

    _url = pLoginConfig->UrlLogin;
    _params.append("account=").append(_account).append("&password=").append(_password);

    std::cout << "account check url:" << _url.c_str() << "?" << _params.c_str() << std::endl;
}

void HttpRequestAccount::ProcessMsg(Json::Value value)
{
    Proto::AccountCheckReturnCode code = Proto::AccountCheckReturnCode::ARC_UNKONWN;
    int httpcode = value["returncode"].asInt();
    if (httpcode == 0)
        code = Proto::AccountCheckReturnCode::ARC_OK;
    else if (httpcode == 2)
        code = Proto::AccountCheckReturnCode::ARC_NOT_FOUND_ACCOUNT;
    else if (httpcode == 3)
        code = Proto::AccountCheckReturnCode::ARC_PASSWORD_WRONG;

    Proto::AccountCheckToHttpRs checkProto;
    checkProto.set_account(_account);
    checkProto.set_return_code(code);

    IMessageSystem::DispatchPacket(Proto::MsgId::MI_AccountCheckToHttpRs, 0, checkProto);
}
