#include "check_time_component.h"
#include "global.h"
#include "log4_help.h"
#include "component_help.h"
#include <thread>

void CheckTimeComponent::CheckBegin()
{
    _beginTick = Global::GetInstance()->TimeTick;
}

void CheckTimeComponent::CheckPoint(std::string key)
{
    if (_maxTicks.find(key) == _maxTicks.end())
    {
        _maxTicks[key] = 0;
        _aveTime[key] = 0;
    }

    auto dis = Global::GetInstance()->TimeTick - _beginTick;
    _aveTime[key] = (_aveTime[key] + dis) * 0.5f;

    if (_maxTicks[key] < dis)
    {
        _maxTicks[key] = dis;

#ifdef LOG_TRACE_COMPONENT_OPEN
        if (_aveTime[key] > 200)
        {
            std::stringstream os;
            os << "key:" << key.c_str() << " time:" << _maxTicks[key] << " ave time:" << _aveTime[key];
            os << " thread id:" << std::this_thread::get_id();
            ComponentHelp::GetTraceComponent()->Trace(TraceType::Time, 0, os.str());
        }
#endif
    }
}
