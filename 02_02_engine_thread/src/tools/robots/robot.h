#pragma once

#include "libserver/network_connector.h"

class Robot : public NetworkConnector {
public:
	bool Init( ) override;
	void RegisterMsgFuntion( ) override;
	void Update( ) override;
};

