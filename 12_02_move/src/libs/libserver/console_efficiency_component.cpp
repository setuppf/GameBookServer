#include "console_efficiency_component.h"
#include "message_system.h"

void ConsoleEfficiencyComponent::Awake()
{
    auto pMsgSystem = GetSystemManager()->GetMessageSystem();

    pMsgSystem->RegisterFunction(this, Proto::MsgId::MI_Efficiency, BindFunP1(this, &ConsoleEfficiencyComponent::HandleEfficiency));
    pMsgSystem->RegisterFunction(this, Proto::MsgId::MI_CmdEfficiency, BindFunP1(this, &ConsoleEfficiencyComponent::HandleCmdEfficiency));
}

void ConsoleEfficiencyComponent::BackToPool()
{
    _threads.clear();
}

void ConsoleEfficiencyComponent::HandleEfficiency(Packet* pPacket)
{
    auto proto = pPacket->ParseToProto<Proto::Efficiency>();
    auto threadId = proto.thread_id();
    auto iter = _threads.find(threadId);
    if (iter == _threads.end())
    {
        ThreadEfficiencyInfo info{};
        info.ThreadTypeKey = (ThreadType)proto.thread_type();
        _threads[threadId] = info;
    }

    auto time = proto.time();
    if (time > _threads[threadId].UpdateTimeMax)
    {
        _threads[threadId].UpdateTimeMax = time;
    }

    _threads[threadId].LastRecvTime = Global::GetInstance()->TimeTick;
    _threads[threadId].UpdateTime = time;
}

void ConsoleEfficiencyComponent::HandleCmdEfficiency(Packet* pPacket)
{
    std::stringstream log;
    log << "\n*************************** " << "\n";
    log << "| id\t";
    log << "| type\t";
    log << "| avg time\t";
    log << "| max time\t";
    log << "| frame\t";
    log << "| log time";
    std::cout << log.str().c_str() << std::endl;

    for (const auto pair : _threads)
    {
        log.clear();
        log.seekg(std::ios::beg);
        log.seekp(std::ios::beg);

        log << pair.first << "\t";
        log << GetThreadTypeName(pair.second.ThreadTypeKey) << "\t";
        log << pair.second.UpdateTime << "\t";
        log << pair.second.UpdateTimeMax << "\t";
        if (pair.second.UpdateTime > 0)
            log << 1000 / pair.second.UpdateTime << "\t";
        else
            log << 1000 / 1 << "\t";
        
        log << timeutil::ToString(pair.second.LastRecvTime);
        std::cout << log.str().c_str() << std::endl;
    }
}
