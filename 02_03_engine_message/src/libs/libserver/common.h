#pragma once

#include <limits>
#include "protobuf/msg.pb.h"
#include "protobuf/proto_id.pb.h"
#define PLATFORM_WIN32			0
#define PLATFORM_UNIX			1

#if defined( __WIN32__ ) || defined( WIN32 ) || defined( _WIN32 )
#  define ENGINE_PLATFORM PLATFORM_WIN32
#else
#  define ENGINE_PLATFORM PLATFORM_UNIX
#endif

#if ENGINE_PLATFORM != PLATFORM_WIN32

#define SOCKET int
#define INVALID_SOCKET -1

#include <stdint.h>
typedef uint64_t	uint64;
typedef uint32_t	uint32;

#else

#define FD_SETSIZE      1024

#include <Ws2tcpip.h>
#include <windows.h>

typedef unsigned __int64	uint64;
typedef unsigned __int32	uint32;

#endif 

enum APP_TYPE
{
	// APP_Global 和 APP_None 值相同， APP_Global 只用于读取配置
	APP_None = 0,
	APP_Global = 0,

	APP_DB_MGR = 1,
	APP_GAME_MGR = 1 << 1,
	APP_SPACE_MGR = 1 << 2,

	APP_LOGIN = 1 << 3,
	APP_GAME = 1 << 4,
	APP_SPACE = 1 << 5,
	APP_ROBOT = 1 << 6,

	APP_APPMGR = APP_GAME_MGR | APP_SPACE_MGR,

	APP_ALL = APP_DB_MGR | APP_GAME_MGR | APP_SPACE_MGR | APP_LOGIN | APP_GAME | APP_SPACE,
};


#define BindFunP4(_self, _f) std::bind ( _f, _self, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4 )
#define BindFunP3(_self, _f) std::bind ( _f, _self, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3 )
#define BindFunP2(_self, _f) std::bind ( _f, _self, std::placeholders::_1, std::placeholders::_2 )
#define BindFunP1(_self, _f) std::bind ( _f, _self, std::placeholders::_1 )
#define BindFunP0(_self, _f) std::bind ( _f, _self)
