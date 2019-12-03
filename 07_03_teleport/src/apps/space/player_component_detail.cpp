#include "player_component_detail.h"

#include "libplayer/player.h"

void PlayerComponentDetail::Awake()
{
	Player* pPlayer = dynamic_cast<Player*>(_parent);
	ParserFromProto(pPlayer->GetPlayerProto());
}

void PlayerComponentDetail::BackToPool()
{
}

void PlayerComponentDetail::ParserFromProto(const Proto::Player& proto)
{
	auto protoBase = proto.base();
	_gender = protoBase.gender();
}

void PlayerComponentDetail::SerializeToProto(Proto::Player* pProto)
{

}

Proto::Gender PlayerComponentDetail::GetGender() const
{
	return _gender;
}
