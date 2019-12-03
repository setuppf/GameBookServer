#include "robot_mgr.h"
#include "libserver/common.h"
#include "libserver/global.h"
#include "libserver/yaml.h"
#include "libserver/entity_system.h"
#include "libserver/message_system.h"
#include "libserver/message_system_help.h"

#include "global_robots.h"
#include <sstream>

void RobotMgr::Awake()
{
    // message
    auto pMsgSystem = GetSystemManager()->GetMessageSystem();
    
    pMsgSystem->RegisterFunction(this, Proto::MsgId::MI_RobotSyncState, BindFunP1(this, &RobotMgr::HandleRobotState));

    AddTimer(0, 2, false, 0, BindFunP0(this, &RobotMgr::ShowInfo));
}

void RobotMgr::BackToPool()
{

}

void RobotMgr::HandleRobotState(Packet* pPacket)
{
    Proto::RobotSyncState protoState = pPacket->ParseToProto<Proto::RobotSyncState>();

    if (_robots.empty() && protoState.states_size() > 0)
    {
        std::cout << "test begin" << std::endl;
        _start = std::chrono::system_clock::now();
    }

    RobotStateType iType = RobotStateType::Space_EnterWorld;
    for (int index = 0; index < protoState.states_size(); index++)
    {
        auto proto = protoState.states(index);
        const auto account = proto.account();
        _robots[account] = RobotStateType(proto.state());
        if (_robots[account] < iType)
        {
            iType = _robots[account];
        }
    }

    _isChange = true;
    NotifyServer(iType);
}

void RobotMgr::NotifyServer(RobotStateType iType)
{
    if (_robots.size() != GlobalRobots::GetInstance()->GetRobotsCount())
        return;

    auto iter = std::find_if(_robots.begin(), _robots.end(), [&iType](auto pair)
        {
            return pair.second < iType;
        });

    if (iter == _robots.end())
    {
        auto end = std::chrono::system_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - _start);
        auto stateName = GetRobotStateTypeShortName(iType);
        std::cout << stateName << " over. time:" << double(duration.count()) * std::chrono::microseconds::period::num / std::chrono::microseconds::period::den << "s" << std::endl;

    }
}

void RobotMgr::ShowInfo()
{
    if (!_isChange)
        return;

    _isChange = false;

    std::map<RobotStateType, int> statData;
    std::for_each(_robots.cbegin(), _robots.cend(), [&statData](auto one)
        {
            auto state = one.second;
            if (statData.find(state) == statData.end())
            {
                statData[state] = 0;
            }

            ++statData[state];
        });

    std::stringstream show;
    auto curTime = timeutil::ToString(Global::GetInstance()->TimeTick);
    show << "++++++++++++++++++++++++++++ " << std::endl << curTime.c_str() << std::endl;

    for (RobotStateType rss = RobotStateType::Http_Connecting; rss < RobotStateType::End; rss = static_cast<RobotStateType>((int)rss + 1))
    {
        if (statData.find(rss) == statData.end())
        {
            show << GetRobotStateTypeName(rss) << 0 << std::endl;
        }
        else
        {
            show << GetRobotStateTypeName(rss) << statData[rss] << std::endl;
        }
    }
    show << "++++++++++++++++++++++++++++" << std::endl;

    std::cout << show.str().c_str() << std::endl;
}
