#include "http_request_account.h"
#include <iostream>

#include "libserver/common.h"
#include "libserver/thread_mgr.h"
#include "libserver/packet.h"
#include "libserver/update_component.h"
#include "libserver/yaml.h"
#include "libserver/component_help.h"

void HttpRequestAccount::Awake(std::string account, std::string password)
{
    // update
    auto pUpdateComponent = AddComponent<UpdateComponent>();
    pUpdateComponent->UpdataFunction = BindFunP0(this, &HttpRequest::Update);

    // 
    _account = account;
    _password = password;
    _curlRs = CRS_None;
    _method = HttpResquestMethod::HRM_Get;
    _state = HttpResquestState::HRS_Send;

    // yaml
    auto pYaml = ComponentHelp::GetYaml();
    const auto pLoginConfig = dynamic_cast<LoginConfig*>(pYaml->GetConfig(APP_LOGIN));

    _url = pLoginConfig->UrlLogin;
    _params.append("account=").append(_account).append("&password=").append(_password);

    // timeout
    AddTimer(1, 10, false, 0, BindFunP0(this, &HttpRequestAccount::ProcessTimeout));
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

    MessageSystemHelp::DispatchPacket(Proto::MsgId::MI_AccountCheckToHttpRs, 0, checkProto);
}
