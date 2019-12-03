#include "app_sync_component.h"
#include "libserver/message_system_help.h"
#include "libserver/message_system.h"
#include "libserver/network_help.h"

void AppSyncComponent::Awake()
{
    AddTimer(0, 2, false, 0, BindFunP0(this, &AppSyncComponent::SyncGameInfoToLogin));

    Json::StreamWriterBuilder jsonBuilder;
    _jsonWriter = jsonBuilder.newStreamWriter();

    // message
    auto pMsgSystem = GetSystemManager()->GetMessageSystem();

    // http
    pMsgSystem->RegisterFunction(this, Proto::MsgId::MI_HttpRequestLogin, BindFunP1(this, &AppSyncComponent::HandleHttpRequestLogin));

    // sync
    pMsgSystem->RegisterFunction(this, Proto::MsgId::MI_AppInfoSync, BindFunP1(this, &AppSyncComponent::HandleAppInfoSync));

    // cmd
    pMsgSystem->RegisterFunction(this, Proto::MsgId::MI_CmdApp, BindFunP1(this, &AppSyncComponent::HandleCmdApp));

    pMsgSystem->RegisterFunction(this, Proto::MsgId::MI_NetworkDisconnect, BindFunP1(this, &AppSyncComponent::HandleNetworkDisconnect));
}

void AppSyncComponent::BackToPool()
{
    delete _jsonWriter;
    _apps.clear();
}

void AppSyncComponent::HandleHttpRequestLogin(Packet* pPacket)
{
    Json::Value responseObj;
    AppInfo info;
    if (!GetOneApp(APP_LOGIN, &info))
    {
        responseObj["returncode"] = (int)Proto::LoginHttpReturnCode::LHRC_NOTFOUND;
        responseObj["ip"] = "";
        responseObj["port"] = 0;
    }
    else
    {
        responseObj["returncode"] = (int)Proto::LoginHttpReturnCode::LHRC_OK;
        responseObj["ip"] = info.Ip;
        responseObj["port"] = info.Port;
    }

    std::stringstream jsonStream;
    _jsonWriter->write(responseObj, &jsonStream);

    MessageSystemHelp::SendHttpResponse(pPacket, jsonStream.str().c_str(), jsonStream.str().length());
}

void AppSyncComponent::HandleAppInfoSync(Packet* pPacket)
{
    AppInfoSyncHandle(pPacket);
}

void AppSyncComponent::HandleNetworkDisconnect(Packet* pPacket)
{
    if (!NetworkHelp::IsTcp(pPacket->GetSocketKey()->NetType))
        return;

    SyncComponent::HandleNetworkDisconnect(pPacket);

    // 有连接断开了，重新向Login发送Space信息
    SyncGameInfoToLogin();
}

void AppSyncComponent::SyncGameInfoToLogin()
{
    Proto::AppInfoListSync proto;
    for (auto pair : _apps)
    {
        auto info = pair.second;
        if ((info.AppType & APP_GAME) == 0)
            continue;

        auto pProto = proto.add_apps();
        pProto->set_app_id(info.AppId);
        pProto->set_app_type(info.AppType);
        pProto->set_online(info.Online);
    }

    // 发送给所有login进程
    if (proto.apps_size() > 0)
    {
        MessageSystemHelp::SendPacketToAllApp(Proto::MsgId::MI_AppInfoListSync, proto, APP_LOGIN);
    }
}
