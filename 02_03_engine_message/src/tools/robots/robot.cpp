#include "robot.h"
#include "libserver/packet.h"

bool Robot::Init()
{
	if (!NetworkConnector::Init())
		return false;

	this->Connect("127.0.0.1", 2233);
	return true;
}

void Robot::RegisterMsgFunction()
{
	NetworkConnector::RegisterMsgFunction();
}

void Robot::Update()
{
	NetworkConnector::Update();

    if (IsConnected() && !_isSendMsg)
    {
        Proto::TestMsg msg;
        msg.set_msg("robot msg");

        auto pResultPacket = new Packet((int)Proto::MsgId::MI_TestMsg, _masterSocket);
        pResultPacket->SerializeToBuffer(msg);

        SendPacket(pResultPacket);
        _isSendMsg = true;
    }
}
