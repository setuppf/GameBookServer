#include "http_request_account.h"
#include <iostream>

#include "libserver/common.h"
#include "libserver/thread_mgr.h"
#include "libserver/packet.h"
#include "libserver/message_system.h"

void HttpRequestAccount::AwakeFromPool(std::string account, std::string password)
{
    _account = account;
    _password = password;
    _curlRs = CRS_None;
    _method = HttpResquestMethod::HRM_Post;

    _url = "192.168.0.120/member_login_t.php";
    _params.append("account=").append(_account).append("&password=").append(_password);

    //std::cout << "account check url:" << _url.c_str() << "?" << _params.c_str() << std::endl;
}

void HttpRequestAccount::ProcessMsg(Json::Value value)
{
    Proto::AccountCheckRs::ReturnCode code = Proto::AccountCheckRs::ARC_UNKONWN;
    int httpcode = value["returncode"].asInt();
    if (httpcode == 0)
        code = Proto::AccountCheckRs::ARC_OK;
    else if (httpcode == 2)
        code = Proto::AccountCheckRs::ARC_NOT_FOUND_ACCOUNT;
    else if (httpcode == 3)
        code = Proto::AccountCheckRs::ARC_PASSWORD_WRONG;

    Proto::AccountCheckToHttpRs checkProto;
    checkProto.set_account(_account);
    checkProto.set_return_code(code);

    auto pCheckPacket = new Packet(Proto::MsgId::MI_AccountCheckToHttpRs, 0);
    pCheckPacket->SerializeToBuffer(checkProto);
    IMessageSystem::DispatchPacket(pCheckPacket);
}
