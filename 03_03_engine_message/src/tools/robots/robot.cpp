#include "robot.h"
#include "libserver/packet.h"
#include "libserver/protobuf/msg.pb.h"
#include "libserver/protobuf/proto_id.pb.h"

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

        auto pPacket = new Packet((int)Proto::MsgId::MI_TestMsg, _masterSocket);
        pPacket->SerializeToBuffer(msg);
        SendPacket(pPacket);
        _isSendMsg = true;
    }
}
