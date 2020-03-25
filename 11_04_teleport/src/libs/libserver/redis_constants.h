#pragma once

// redis 关健字

// 登录 Token
#define RedisKeyAccountTokey  "engine::token::"
#define RedisKeyAccountTokeyTimeout  2 * 60

// 角色登录
#define RedisKeyAccountOnlineLogin  "engine::online::login::"
#define RedisKeyAccountOnlineLoginTimeout  6 * 60

// 角色在线
#define RedisKeyAccountOnlineGame  "engine::online::game::"
#define RedisKeyAccountOnlineGameTimeout  6 * 60
