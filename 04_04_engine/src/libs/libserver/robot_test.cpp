#include "robot_test.h"
#include "robot_state_type.h"
#include "packet.h"

#include <iostream>

void RobotTest::RegisterMsgFunction()
{
    auto pMsgCallBack = new MessageCallBackFunction();
    AttachCallBackHandler(pMsgCallBack);

    pMsgCallBack->RegisterFunction(Proto::MsgId::MI_RobotTestBegin, BindFunP1(this, &RobotTest::HandleTestBegin));
    pMsgCallBack->RegisterFunction(Proto::MsgId::MI_RobotTestEnd, BindFunP1(this, &RobotTest::HandleTestEnd));
}

void RobotTest::HandleTestBegin(Packet* pPacket)
{
    std::cout << "test begin" << std::endl;
    _start = std::chrono::system_clock::now();
}

void RobotTest::HandleTestEnd(Packet* pPacket)
{
    auto proto = pPacket->ParseToProto<Proto::RobotTestEnd>();
    auto end = std::chrono::system_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - _start);
    auto stateName = GetRobotStateTypeShortName((RobotStateType)proto.state());
    std::cout << stateName << " over. time:" << double(duration.count()) * std::chrono::microseconds::period::num / std::chrono::microseconds::period::den << "s" << std::endl;
}
