#include "player_component_token.h"
#include "libserver/log4_help.h"

void PlayerComponentToken::Awake(const std::string token)
{
	_token = token;
}

void PlayerComponentToken::BackToPool()
{
	_token = "";
}

bool PlayerComponentToken::IsTokenValid(std::string token) const
{
	return strncmp(_token.c_str(), token.c_str(), _token.length()) == 0;
}
