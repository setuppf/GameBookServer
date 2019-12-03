#include "console_app_component.h"
#include "login_sync_component.h"

#include "libserver/message_component.h"

void ConsoleAppComponent::Awake()
{
    auto pMsgCallBack = new MessageCallBackFunction();
    AddComponent<MessageComponent>(pMsgCallBack);
    pMsgCallBack->RegisterFunction(Proto::MsgId::MI_CmdApp, BindFunP1(this, &ConsoleAppComponent::HandleCmdApp));
}

void ConsoleAppComponent::BackToPool()
{
}

void ConsoleAppComponent::HandleCmdApp(Packet* pPacket)
{
    auto cmdProto = pPacket->ParseToProto<Proto::CmdApp>();
    auto cmdType = cmdProto.cmd_type();
    if (cmdType == Proto::CmdApp_CmdType_Info)
    {
        HandleCmdAppInfo();
    }
}

void ConsoleAppComponent::HandleCmdAppInfo()
{
    auto pSyncEntity = GetSystemManager()->GetEntitySystem()->GetComponent<SyncComponent>();
    if (pSyncEntity == nullptr)
        return;

    pSyncEntity->CmdShow();
}
