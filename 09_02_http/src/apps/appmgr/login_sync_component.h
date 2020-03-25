#pragma once

#include "libserver/sync_component.h"
#include "libserver/system.h"
#include <json/writer.h>

class LoginSyncComponent :public SyncComponent, public IAwakeSystem<>
{
public:
    void Awake() override;
    void BackToPool() override;

protected:
    void HandleHttpRequestLogin(Packet* pPacket);

private:
    Json::StreamWriter* _jsonWriter{ nullptr };
};

