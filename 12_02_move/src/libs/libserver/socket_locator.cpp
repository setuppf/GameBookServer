#include "socket_locator.h"
#include "message_system.h"

void SocketLocator::Awake()
{
    auto pMsgSystem = GetSystemManager()->GetMessageSystem();
    pMsgSystem->RegisterFunction(this, Proto::MsgId::MI_NetworkDisconnect, BindFunP1(this, &SocketLocator::HandleNetworkDisconnect));
}

void SocketLocator::BackToPool()
{
    _componentes.clear();
}

void SocketLocator::RegisterToLocator(SOCKET socket, uint64 sn)
{
    std::lock_guard<std::mutex> guard(_lock);
    const auto iter = _componentes.find(socket);
    if (iter != _componentes.end())
    {
        _componentes[socket] = sn;
    }
    else
    {
        _componentes.insert(std::make_pair(socket, sn));
    }
}

void SocketLocator::Remove(const SOCKET socket)
{
    std::lock_guard<std::mutex> guard(_lock);
    _componentes.erase(socket);
}

uint64 SocketLocator::GetTargetEntitySn(const SOCKET socket)
{
    std::lock_guard<std::mutex> guard(_lock);
    const auto  iter = _componentes.find(socket);
    if (iter == _componentes.end())
        return 0;

    return iter->second;
}

void SocketLocator::HandleNetworkDisconnect(Packet* pPacket)
{
    std::lock_guard<std::mutex> guard(_lock);
    _componentes.erase(pPacket->GetSocketKey()->Socket);
}

