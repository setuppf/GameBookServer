#include "trace_component.h"
#include "global.h"
#include "log4_help.h"

void TraceDetail::Trace(const std::string& trace)
{
    _details.emplace_back(timeutil::ToString(Global::GetInstance()->TimeTick) + " " + trace);
}

void TraceDetail::Show()
{
    for (auto one : _details)
    {
        std::cout << one << std::endl;
    }
}

void TraceComponent::Awake()
{

}

void TraceComponent::BackToPool()
{
    _traces.clear();
}

void TraceComponent::TraceAccount(std::string account, SOCKET socket)
{
    std::lock_guard<std::mutex> guard(_lock);

    const auto iter = _accounts.find(account);
    if (iter == _accounts.end())
    {
        _accounts.insert(std::make_pair(account, std::set<SOCKET>()));
    }

    _accounts[account].insert(socket);
}

void TraceComponent::ShowAccount(const std::string& account)
{
    std::lock_guard<std::mutex> guard(_lock);

    const auto iter = _accounts.find(account);
    if (iter == _accounts.end())
    {
        LOG_WARN("not found. account:" << account.c_str());
        return;
    }

    std::cout << "socket:";
    for (auto one : iter->second)
    {
        std::cout << " " << one;
    }

    std::cout << std::endl;
}

void TraceComponent::Trace(TraceType iType, int key, const std::string& trace)
{
    std::lock_guard<std::mutex> guard(_lock);

    const auto iter = _traces.find(iType);
    if (iter == _traces.end())
    {
        _traces.insert(std::make_pair(iType, std::map<int, TraceDetail>()));
    }

    const auto iterKey = _traces[iType].find(key);
    if (iterKey == _traces[iType].end())
    {
        _traces[iType].insert(std::make_pair(key, TraceDetail()));
    }

    _traces[iType][key].Trace(trace);

    //if (iType == TraceType::Connector || iType == TraceType::Player)
    //    LOG_DEBUG("key:" << key << " " << trace.c_str());
}

void TraceComponent::Show(TraceType iType, int key)
{
    std::lock_guard<std::mutex> guard(_lock);

    const auto iter = _traces.find(iType);
    if (iter == _traces.end())
        return;

    auto keyTrace = _traces[iType];
    auto iterKey = keyTrace.find(key);
    if (iterKey == keyTrace.end())
        return;

    std::cout << " key:" << iterKey->first << std::endl;
    iterKey->second.Show();
}

void TraceComponent::Clean()
{
    std::lock_guard<std::mutex> guard(_lock);
    _traces.clear();
}
