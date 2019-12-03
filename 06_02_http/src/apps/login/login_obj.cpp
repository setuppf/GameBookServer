#include "login_obj.h"
#include "libserver/message_system_help.h"

LoginObj::LoginObj(NetworkIdentify* pIdentify, std::string account, std::string password)
{
    _account = account;
    _password = password;

    _socketKey = pIdentify->GetSocketKey();
    _objKey = { ObjectKeyType::Account, ObjectKeyValue{0, _account}};

    // 登录成功，修改网络底层的标识
    MessageSystemHelp::DispatchPacket(Proto::MsgId::MI_NetworkListenKey, this);
}

std::string LoginObj::GetAccount() const
{
    return _account;
}

std::string LoginObj::GetPassword() const
{
    return _password;
}

   