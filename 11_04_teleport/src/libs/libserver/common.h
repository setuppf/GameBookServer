#pragma once

#include <climits>
#include "protobuf/proto_id.pb.h"
#include "protobuf/db.pb.h"
#include "protobuf/msg.pb.h"

#ifndef PATH_MAX  
#define PATH_MAX 1024  
#endif  

#define INVALID_ID (-1)

#define PLATFORM_WIN32			0
#define PLATFORM_UNIX			1

#if defined( __WIN32__ ) || defined( WIN32 ) || defined( _WIN32 )
#  define ENGINE_PLATFORM PLATFORM_WIN32
#else
#  define ENGINE_PLATFORM PLATFORM_UNIX
#endif

#if ENGINE_PLATFORM != PLATFORM_WIN32
#include <string.h>
#include <stdint.h>
#define engine_stricmp strcasecmp
#define engine_access access
#define engine_strncpy(dest, destsz, src, srcsz) strncpy(dest, src, srcsz)

#else
#define engine_stricmp _stricmp
#define engine_access _access
#define engine_strncpy(dest, destsz, src, srcsz) strncpy_s(dest, destsz, src, srcsz)

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

typedef unsigned __int64	uint64;
typedef unsigned __int32	uint32;

#endif 

#define BindFunP4(_self, _f) std::bind ( _f, _self, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4 )
#define BindFunP3(_self, _f) std::bind ( _f, _self, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3 )
#define BindFunP2(_self, _f) std::bind ( _f, _self, std::placeholders::_1, std::placeholders::_2 )
#define BindFunP1(_self, _f) std::bind ( _f, _self, std::placeholders::_1 )
#define BindFunP0(_self, _f) std::bind ( _f, _self)
