#include "player_component_last_map.h"
#include "libresource/resource_manager.h"
#include "player.h"
#include "libresource/resource_help.h"

LastWorld::LastWorld(const int worldId, const uint64 worldSn, const Vector3 pos)
{
	this->WorldId = worldId;
	this->WorldSn = worldSn;
	this->Position = pos;

	//LOG_DEBUG("Init. LastWorld. id:" << this->WorldId << " sn:" << this->WorldSn << " pos:" << this->position.ToString().c_str());
}

LastWorld::LastWorld(Proto::LastWorld proto)
{
	this->WorldId = proto.world_id();
	this->WorldSn = proto.world_sn();
	this->Position.ParserFromProto(proto.position());

	//LOG_DEBUG("ParserFromProto. LastWorld. id:" << this->WorldId << " sn:" << this->WorldSn << " pos:" << this->position.ToString().c_str());
}

void LastWorld::SerializeToProto(Proto::LastWorld* pProto) const
{
	pProto->set_world_id(WorldId);
	pProto->set_world_sn(WorldSn);
	Position.SerializeToProto(pProto->mutable_position());

	//LOG_DEBUG("SerializeToProto. LastWorld. id:" << this->WorldId << " sn:" << this->WorldSn << " pos:" << this->position.ToString().c_str());
}

void PlayerComponentLastMap::Awake()
{
	Player* pPlayer = static_cast<Player*>(_parent);
	ParserFromProto(pPlayer->GetPlayerProto());
}

void PlayerComponentLastMap::ParserFromProto(const Proto::Player& proto)
{
	// 公共地图
	auto protoMap = proto.misc().last_world();
	int worldId = protoMap.world_id();
    auto pResMgr = ResourceHelp::GetResourceManager();
    auto pMap = pResMgr->Worlds->GetResource(worldId);
	if (pMap != nullptr)
	{
		_pPublic = new LastWorld(protoMap);
	}
	else
	{
		pMap = pResMgr->Worlds->GetInitMap();
		_pPublic = new LastWorld(pMap->GetId(), 0, pMap->GetInitPosition());
	}

	// 副本
	auto protoDungeon = proto.misc().last_dungeon();
	worldId = protoDungeon.world_id();
	pMap = pResMgr->Worlds->GetResource(worldId);
	if (pMap != nullptr)
	{
		_pDungeon = new LastWorld(protoDungeon);
	}
}

void PlayerComponentLastMap::SerializeToProto(Proto::Player* pProto)
{
	if (_pPublic != nullptr)
	{
		const auto pLastMap = pProto->mutable_misc()->mutable_last_world();
		_pPublic->SerializeToProto(pLastMap);
	}

	if (_pDungeon != nullptr)
	{
		const auto pLastDungeon = pProto->mutable_misc()->mutable_last_dungeon();
		_pDungeon->SerializeToProto(pLastDungeon);
	}
}

LastWorld* PlayerComponentLastMap::GetLastPublicMap() const
{
	return _pPublic;
}

LastWorld* PlayerComponentLastMap::GetLastDungeon() const
{
	return _pDungeon;
}

LastWorld* PlayerComponentLastMap::GetCur() const
{
	if (_pPublic->WorldId == _curWorldId)
		return _pPublic;

	return _pDungeon;
}

void PlayerComponentLastMap::EnterWorld(const int worldId, const uint64 worldSn)
{
	auto pResMgr = ResourceHelp::GetResourceManager();
	const auto pWorldRes = pResMgr->Worlds->GetResource(worldId);
	_curWorldId = worldId;
	if (pWorldRes->IsType(ResourceWorldType::Public))
	{
		auto pLastMap = GetLastPublicMap();
		if (pLastMap->WorldId == worldId)
		{
			// 上次的位置
			pLastMap->WorldId = worldId;
			pLastMap->WorldSn = worldSn;
		}
		else
		{
			pLastMap->WorldId = worldId;
			pLastMap->WorldSn = worldSn;
			pLastMap->Position = pWorldRes->GetInitPosition();
		}
	}
	else
	{
		EnterDungeon(worldId, _sn, pWorldRes->GetInitPosition());
	}
}

void PlayerComponentLastMap::SetCurWorld(int worldId)
{
	_curWorldId = worldId;
}

void PlayerComponentLastMap::EnterDungeon(const int worldId, const uint64 worldSn, const Vector3 position)
{
	if (_pDungeon == nullptr)
	{
		_pDungeon = new LastWorld(worldId, worldSn, position);
	}

	if (_pDungeon->WorldSn != worldSn)
	{
		_pDungeon->WorldId = worldId;
		_pDungeon->WorldSn = worldSn;
		_pDungeon->Position = position;
	}
}

void PlayerComponentLastMap::BackToPool()
{
	if (_pPublic != nullptr)
	{
		delete _pPublic;
		_pPublic = nullptr;
	}

	if (_pDungeon != nullptr)
	{
		delete _pDungeon;
		_pDungeon = nullptr;
	}
}
