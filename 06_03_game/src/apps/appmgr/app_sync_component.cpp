#include "app_sync_component.h"
#include "libserver/message_component.h"
#include "libserver/message_system_help.h"

void AppSyncComponent::Awake()
{
    AddTimer(0, 2, false, 0, BindFunP0(this, &AppSyncComponent::SyncGameInfoToLogin));

    Json::StreamWriterBuilder jsonBuilder;
    _jsonWriter = jsonBuilder.newStreamWriter();

    // message
    auto pMsgCallBack = new MessageCallBackFunction();
    AddComponent<MessageComponent>(pMsgCallBack);

    // http
    pMsgCallBack->RegisterFunction(Proto::MsgId::MI_HttpRequestLogin, BindFunP1(this, &AppSyncComponent::HandleHttpRequestLogin));

    // sync
    pMsgCallBack->RegisterFunction(Proto::MsgId::MI_AppInfoSync, BindFunP1(this, &AppSyncComponent::HandleAppInfoSync));

    // cmd
    pMsgCallBack->RegisterFunction(Proto::MsgId::MI_CmdApp, BindFunP1(this, &AppSyncComponent::HandleCmdApp));
}

void AppSyncComponent::BackToPool()
{
    delete _jsonWriter;
    _apps.clear();
}

void AppSyncComponent::HandleCmdApp(Packet* pPacket)
{
    auto cmdProto = pPacket->ParseToProto<Proto::CmdApp>();
    auto cmdType = cmdProto.cmd_type();
    if (cmdType == Proto::CmdApp_CmdType_Info)
    {
        CmdShow();
    }
}

void AppSyncComponent::HandleHttpRequestLogin(Packet* pPacket)
{
    Json::Value responseObj;
    AppInfo info;
    if (!GetOneApp(APP_LOGIN, &info))
    {
        responseObj["returncode"] = (int)Proto::LoginHttpReturnCode::LHRC_TIMEOUT;
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
    MessageSystemHelp::SendPacketToAllApp(Proto::MsgId::MI_AppInfoListSync, proto, APP_LOGIN);
}
