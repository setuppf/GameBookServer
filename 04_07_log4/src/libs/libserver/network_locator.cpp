#include "network_locator.h"
#include "network_listen.h"
#include <algorithm>

void NetworkLocator::BackToPool()
{
	_connectors.clear();
	_listens.clear();
}

void NetworkLocator::AddConnectorLocator(INetwork* pNetwork, APP_TYPE appType, int appId)
{
	std::lock_guard<std::mutex> guard(_lock);
	auto iter = _connectors.find(appType);
	if (iter == _connectors.end())
	{
		_connectors[appType] = std::map<int, INetwork*>();
	}

	_connectors[appType][appId] = pNetwork;
}

void NetworkLocator::AddListenLocator(INetwork* pNetwork, NetworkType networkType)
{
	std::lock_guard<std::mutex> guard(_lock);
	_listens[networkType] = pNetwork;
}

INetwork* NetworkLocator::GetListen(NetworkType networkType)
{
	std::lock_guard<std::mutex> guard(_lock);
	auto iter = _listens.find(networkType);
	if (iter == _listens.end())
		return nullptr;

	return iter->second;
}

INetwork* NetworkLocator::GetNetworkConnector(const SOCKET socket)
{
	std::lock_guard<std::mutex> guard(_lock);

	auto iter = _connectors.begin();
	while (iter != _connectors.end())
	{
		auto& mapObj = iter->second;
		auto iterSub = std::find_if(mapObj.begin(), mapObj.end(), [&socket](auto pair)
		{
			if (pair.second->GetSocket() == socket)
				return true;

			return false;
		});

		if (iterSub != mapObj.end())
		{
			return iterSub->second;
		}

		++iter;
	}

	return nullptr;
}

INetwork* NetworkLocator::GetNetworkConnector(const APP_TYPE appType, const int appId)
{
	std::lock_guard<std::mutex> guard(_lock);

	auto iter = _connectors.find(appType);
	if (iter == _connectors.end())
		return nullptr;

	auto& mapObj = iter->second;
	auto iterSub = mapObj.find(appId);
	if (iterSub == mapObj.end())
		return nullptr;

	return iterSub->second;
}

std::tuple<APP_TYPE, int> NetworkLocator::GetNetworkConnectorInfo(const SOCKET socket)
{
	std::lock_guard<std::mutex> guard(_lock);

	auto iter = _connectors.begin();
	while (iter != _connectors.end())
	{
		auto& mapObj = iter->second;
		auto iterSub = std::find_if(mapObj.begin(), mapObj.end(), [&socket](auto pair)
		{
			if (pair.second->GetSocket() == socket)
				return true;

			return false;
		});

		if (iterSub != mapObj.end())
		{
			return std::tuple<APP_TYPE, int>(iter->first, iterSub->first);
		}

		++iter;
	}

	return std::tuple<APP_TYPE, int>(APP_None, 0);
}

struct RetrieveSecond
{
	template <typename T>
	typename T::second_type operator()(T keyValuePair) const
	{
		return keyValuePair.second;
	}
};

std::list<INetwork*> NetworkLocator::GetNetworks(const APP_TYPE appType)
{
	std::lock_guard<std::mutex> guard(_lock);

	std::list<INetwork*> rs;
	auto iter = _connectors.find(appType);
	if (iter != _connectors.end())
	{
		auto& mapObj = iter->second;
		std::transform(mapObj.begin(), mapObj.end(), std::back_inserter(rs), RetrieveSecond());
	}

	return rs;
}

int NetworkLocator::GetNetworkAppId(const SOCKET socket)
{
	std::lock_guard<std::mutex> guard(_lock);

	auto iter = _connectors.begin();
	while (iter != _connectors.end())
	{
		auto& mapObj = iter->second;
		auto iterSub = std::find_if(mapObj.begin(), mapObj.end(), [&socket](auto pair)
		{
			if (pair.second->GetSocket() == socket)
				return true;

			return false;
		});

		if (iterSub != mapObj.end())
		{
			return iterSub->first;
		}

		++iter;
	}

	return 0;
}

APP_TYPE NetworkLocator::GetNetworkAppType(const int socket)
{
	std::lock_guard<std::mutex> guard(_lock);

	auto iter = _connectors.begin();
	while (iter != _connectors.end())
	{
		auto& mapObj = iter->second;
		auto iterSub = std::find_if(mapObj.begin(), mapObj.end(), [&socket](auto pair)
		{
			if (pair.second->GetSocket() == socket)
				return true;

			return false;
		});

		if (iterSub != mapObj.end())
		{
			return iter->first;
		}
		++iter;
	}

	return APP_None;
}
