#include "efficiency_thread_component.h"
#include "message_system_help.h"
#include "thread_mgr.h"
#include "message_system.h"

void EfficiencyThreadComponent::Awake(ThreadType threadType, std::thread::id threadId)
{
	_threadType = threadType;
	_efficiencyUpdateTime = 0;

    std::stringstream ss;
    ss << threadId;
    _threadId = ss.str();

    AddTimer(0, ThreadEfficiencyTime, false, 0, BindFunP0(this, &EfficiencyThreadComponent::Sync));
}

void EfficiencyThreadComponent::BackToPool()
{
	_efficiencyUpdateTime = 0;
}

void EfficiencyThreadComponent::UpdateTime(const uint64 disTime)
{
	const auto puls = (_efficiencyUpdateTime + disTime) * 0.5f;
	_efficiencyUpdateTime = puls;
}

void EfficiencyThreadComponent::Sync() const
{
	// Ð§ÂÊ²âÊÔ
	Proto::Efficiency protoTest;
    protoTest.set_time(_efficiencyUpdateTime);
    protoTest.set_thread_type((int)_threadType);
    protoTest.set_thread_id(_threadId);

    MessageSystemHelp::DispatchPacket(Proto::MI_Efficiency, protoTest, nullptr);
}
