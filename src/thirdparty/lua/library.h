
#ifndef __SRC_LUA_LIBRARY_H__
#define __SRC_LUA_LIBRARY_H__

extern "C"
{

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

// vec3
LUALIB_API int luaopen_vec3( lua_State * L );

// protobuf
LUALIB_API int luaopen_pb( lua_State * L );

// luaprofiler
LUALIB_API int luaopen_libplua( lua_State * L );

// rapidjson
LUALIB_API int luaopen_rapidjson( lua_State * L );

// fixpoint
LUALIB_API int luaopen_fixpoint( lua_State * L );

}

#endif
