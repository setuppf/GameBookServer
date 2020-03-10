#include "login_sync_component.h"
#include "libserver/message_component.h"

void LoginSyncComponent::Awake()
{
    auto pMsgCallBack = new MessageCallBackFunction();
    AddComponent<MessageComponent>(pMsgCallBack);

    pMsgCallBack->RegisterFunction(Proto::MsgId::MI_AppInfoSync, BindFunP1(this, &LoginSyncComponent::AppInfoSyncHandle));
}

void LoginSyncComponent::BackToPool()
{
    _apps.clear();
}
