#include "login_obj_mgr.h"

#include "libserver/log4_help.h"
#include "libserver/global.h"
#include "libserver/component_help.h"

void LoginObjMgr::BackToPool()
{
    auto iter = _players.begin();
    while (iter != _players.end())
    {
        delete iter->second;
        ++iter;
    }

    _players.clear();
    _accounts.clear();
}

LoginObj* LoginObjMgr::AddPlayer(NetworkIdentify* pIdentify, std::string account, std::string password)
{
    auto socket = pIdentify->GetSocketKey().Socket;
    const auto iter = _players.find(socket);
    if (iter != _players.end())
    {
        LOG_WARN("failed to add player. " << pIdentify << " account:" << account.c_str() << " existing account:" << iter->second->GetAccount().c_str());

#ifdef LOG_TRACE_COMPONENT_OPEN
        const auto traceMsg = std::string("failed to add player.")
            .append(" account:").append(account.c_str())
            .append(" existing account:").append(iter->second->GetAccount().c_str());

        ComponentHelp::GetTraceComponent()->Trace(TraceType::Player, socket, traceMsg);
#endif

        return nullptr;
    }

    _accounts[account] = socket;
    _players[socket] = new LoginObj(pIdentify, account, password);

#ifdef LOG_TRACE_COMPONENT_OPEN
    const auto traceMsg = std::string("add player.").append(" account:").append(account.c_str());
    ComponentHelp::GetTraceComponent()->Trace(TraceType::Player, socket, traceMsg);
#endif

    return _players[socket];
}

void LoginObjMgr::RemovePlayer(SOCKET socket)
{
    auto iter = _players.find(socket);
    if (iter == _players.end())
    {
        LOG_WARN("player offline. can't find player by socket:" << socket);

#ifdef LOG_TRACE_COMPONENT_OPEN
        const auto traceMsg = std::string("player offline. can't find player by socket.");
        ComponentHelp::GetTraceComponent()->Trace(TraceType::Player, socket, traceMsg);
#endif

        return;
    }

    LoginObj* pPlayer = iter->second;

    _players.erase(socket);
    _accounts.erase(pPlayer->GetAccount());

#ifdef LOG_TRACE_COMPONENT_OPEN
    const auto traceMsg = std::string("player offline. player account:").append(pPlayer->GetAccount());
    ComponentHelp::GetTraceComponent()->Trace(TraceType::Player, socket, traceMsg);
#endif

    delete pPlayer;
}

LoginObj* LoginObjMgr::GetPlayer(SOCKET socket)
{
    auto iter = _players.find(socket);
    if (iter == _players.end())
        return nullptr;

    return iter->second;
}

LoginObj* LoginObjMgr::GetPlayer(std::string account)
{
    auto iter = _accounts.find(account);
    if (iter == _accounts.end())
        return nullptr;

    SOCKET socket = iter->second;
    auto iterPlayer = _players.find(socket);
    if (iterPlayer == _players.end())
    {
        _accounts.erase(account);
        return nullptr;
    }

    return iterPlayer->second;
}

size_t LoginObjMgr::Count()
{
    return _players.size();
}
