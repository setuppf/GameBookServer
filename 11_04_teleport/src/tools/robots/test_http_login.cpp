#include "test_http_login.h"
#include "libserver/message_system_help.h"
#include "libserver/component_help.h"
#include "libserver/message_system.h"

void TestHttpLogin::Awake(std::string account, std::string password)
{
    _account = account;
    _password = password;

    _tagKey.AddTag(TagType::Account, _account);

    // message
    auto pMsgSystem = GetSystemManager()->GetMessageSystem();

    // 处理断线
    pMsgSystem->RegisterFunctionFilter<TestHttpLogin>(this, Proto::MsgId::MI_NetworkDisconnect, BindFunP1(this, &TestHttpLogin::GetTestHttpLogin), BindFunP2(this, &TestHttpLogin::HandleNetworkDisconnect));
    pMsgSystem->RegisterFunctionFilter<TestHttpLogin>(this, Proto::MsgId::MI_NetworkConnected, BindFunP1(this, &TestHttpLogin::GetTestHttpLogin), BindFunP2(this, &TestHttpLogin::HandleNetworkConnected));

    pMsgSystem->RegisterFunctionFilter<TestHttpLogin>(this, Proto::MsgId::MI_HttpOuterResponse, BindFunP1(this, &TestHttpLogin::GetTestHttpLogin), BindFunP2(this, &TestHttpLogin::HandleHttpOuterResponse));

    // 发起连接
    auto pYaml = ComponentHelp::GetYaml();
    const auto pLoginConfig = dynamic_cast<LoginConfig*>(pYaml->GetConfig(APP_LOGIN));
    ParseUrlInfo info;
    if (!MessageSystemHelp::ParseUrl(pLoginConfig->UrlLogin, info))
    {
        LOG_ERROR("parse login url failed. url:" << pLoginConfig->UrlLogin.c_str());
        return;
    }

    _ip = info.Host;
    _port = info.Port;
    _mothed = info.Mothed;

    TagValue tagValue{ _account, 0L };
    MessageSystemHelp::CreateConnect(NetworkType::HttpConnector, TagType::Account, tagValue, _ip, _port);
}

void TestHttpLogin::BackToPool()
{

}

TestHttpLogin* TestHttpLogin::GetTestHttpLogin(NetIdentify* pNetIdentify)
{
    auto pTagValue = pNetIdentify->GetTagKey()->GetTagValue(TagType::Account);
    if (pTagValue == nullptr)
        return nullptr;

    if (pTagValue->KeyStr == _account)
        return this;

    return nullptr;
}

void TestHttpLogin::HandleNetworkDisconnect(TestHttpLogin* pObj, Packet* pPacket)
{
    LOG_ERROR("test http login. recv network disconnect.");
}

void TestHttpLogin::HandleNetworkConnected(TestHttpLogin* pObj, Packet* pPacket)
{
    // 连接成功，发送数据
    _socketKey.CopyFrom(pPacket->GetSocketKey());
    LOG_DEBUG("connected." << pPacket);

    std::map<std::string, std::string> params;
    params["account"] = _account;
    params["password"] = _password;
    MessageSystemHelp::SendHttpRequest(this, _ip, _port, _mothed, &params);
}

void TestHttpLogin::HandleHttpOuterResponse(TestHttpLogin* pObj, Packet* pPacket)
{
    auto protoHttp = pPacket->ParseToProto<Proto::Http>();
    LOG_DEBUG("http code:" << protoHttp.status_code());
    LOG_DEBUG(protoHttp.body().c_str());
}
