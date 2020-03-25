#include "player_collector_component.h"
#include "player.h"

void PlayerCollectorComponent::Awake()
{

}

void PlayerCollectorComponent::BackToPool()
{
    auto pEntitySystem = GetSystemManager()->GetEntitySystem();
    auto iter = _players.begin();
    while (iter != _players.end())
    {
        pEntitySystem->RemoveComponent(iter->second);
        ++iter;
    }

    _accounts.clear();
    _players.clear();
}

Player* PlayerCollectorComponent::AddPlayer(NetIdentify* pIdentify, const std::string account)
{
    const auto socket = pIdentify->GetSocketKey()->Socket;
	const auto iter = _players.find(socket);
	if (iter != _players.end())
	{
		std::cout << "AddPlayer error." << std::endl;
		return nullptr;
	}

	_accounts[account] = socket;
	const auto pPlayer = GetSystemManager()->GetEntitySystem()->AddComponent<Player>(pIdentify, account);
	_players[socket] = pPlayer;
	return pPlayer;
}

void PlayerCollectorComponent::RemovePlayerBySocket(SOCKET socket)
{
	const auto iter = _players.find(socket);
	if (iter == _players.end())
		return;

	Player* pPlayer = iter->second;

	_players.erase(socket);
	_accounts.erase(pPlayer->GetAccount());

    GetSystemManager()->GetEntitySystem()->RemoveComponent(pPlayer);
}

void PlayerCollectorComponent::RemovePlayerBySn(uint64 playerSn)
{
	auto iter = std::find_if(_players.begin(), _players.end(), [&playerSn](auto pair)
	{
        return pair.second->GetPlayerSN() == playerSn;
	});

	if (iter == _players.end())
		return;

	RemovePlayerBySocket(iter->second->GetSocketKey()->Socket);
}

Player* PlayerCollectorComponent::GetPlayerBySocket(const SOCKET socket)
{
	const auto iter = _players.find(socket);
	if (iter == _players.end())
		return nullptr;

	return iter->second;
}

Player* PlayerCollectorComponent::GetPlayerByAccount(const std::string account)
{
	const auto iter = _accounts.find(account);
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

Player* PlayerCollectorComponent::GetPlayerBySn(uint64 playerSn)
{
	auto iter = std::find_if(_players.begin(), _players.end(), [&playerSn](auto pair)
	{
        return pair.second->GetPlayerSN() == playerSn;
	});

	if (iter == _players.end())
		return nullptr;

	return iter->second;
}

int PlayerCollectorComponent::OnlineSize() const
{
	return _players.size();
}

std::map<SOCKET, Player*>& PlayerCollectorComponent::GetAll()
{
	return _players;
}
