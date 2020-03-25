#pragma once

#include "libserver/network_connector.h"

class Robot : public NetworkConnector {
public:
    bool Init() override;
    void RegisterMsgFunction() override;
    void Update() override;

private:
    bool _isSendMsg{ false };
};

