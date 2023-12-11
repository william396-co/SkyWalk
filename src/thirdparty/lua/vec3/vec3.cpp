
#include <cassert>
#include <string>

#include "lua/library.h"
#include "domain/vec3.h"

static const char VEC3META[] = "Vec3::meta";

class Vec3Helper
{
public :
    template<typename T>
        static bool pop( lua_State * ls, int index, T & v3 )
        {
            int type = lua_type( ls, index );

            if ( type == LUA_TNUMBER )
            {
                v3.x = lua_tonumber( ls, index );
                v3.y = v3.x;
                v3.z = v3.x;
            }
            else if ( type == LUA_TTABLE )
            {
                lua_getfield( ls, index, "x" );
                v3.x = lua_tonumber( ls, -1 );
                lua_getfield( ls, index, "y" );
                v3.y = lua_tonumber( ls, -1 );
                lua_getfield( ls, index, "z" );
                v3.z = lua_tonumber( ls, -1 );
            }

            return true;
        }

    template<typename T>
        static void push( lua_State * ls, const T & v3 )
        {
            lua_newtable( ls );

            lua_pushnumber( ls, v3.x );
            lua_setfield( ls, -2, "x" );
            lua_pushnumber( ls, v3.y );
            lua_setfield( ls, -2, "y" );
            lua_pushnumber( ls, v3.z );
            lua_setfield( ls, -2, "z" );

            luaL_getmetatable( ls, VEC3META );
            lua_setmetatable( ls, -2 );
        }
};

template<> bool Vec3Helper::pop( lua_State * ls, int index, Vec3i & v3 )
{
    bool rc = true;
    int type = lua_type( ls, index );

    if ( type == LUA_TNUMBER )
    {
        v3.x = lua_tointeger( ls, index );
        v3.y = v3.x;
        v3.z = v3.x;
    }
    else if ( type == LUA_TTABLE )
    {
        lua_getfield( ls, index, "x" );
        rc &= lua_isinteger( ls, -1 );
        v3.x = lua_tointeger( ls, -1 );

        lua_getfield( ls, index, "y" );
        rc &= lua_isinteger( ls, -1 );
        v3.y = lua_tointeger( ls, -1 );

        lua_getfield( ls, index, "z" );
        rc &= lua_isinteger( ls, -1 );
        v3.z = lua_tointeger( ls, -1 );
    }

    return rc;
}

template<> void Vec3Helper::push( lua_State * ls, const Vec3i & v3 )
{
    lua_newtable( ls );

    lua_pushinteger( ls, v3.x );
    lua_setfield( ls, -2, "x" );
    lua_pushinteger( ls, v3.y );
    lua_setfield( ls, -2, "y" );
    lua_pushinteger( ls, v3.z );
    lua_setfield( ls, -2, "z" );

    luaL_getmetatable( ls, VEC3META );
    lua_setmetatable( ls, -2 );
}

static int vec3_add( lua_State * ls )
{
    Vec3i v1, v2;

    Vec3Helper::pop( ls, 1, v1 );
    Vec3Helper::pop( ls, 2, v2 );

    Vec3Helper::push( ls, v1+v2 );

    return 1;
}

static int vec3_sub( lua_State * ls )
{
    Vec3i v1, v2;

    Vec3Helper::pop( ls, 1, v1 );
    Vec3Helper::pop( ls, 2, v2 );

    Vec3Helper::push( ls, v1-v2 );

    return 1;
}

static int vec3_mul( lua_State * ls )
{
    Vec3i v1, v2;

    Vec3Helper::pop( ls, 1, v1 );
    Vec3Helper::pop( ls, 2, v2 );

    Vec3Helper::push( ls, v1*v2 );

    return 1;
}

static int vec3_idiv( lua_State * ls )
{
    Vec3i v1, v2;

    Vec3Helper::pop( ls, 1, v1 );
    Vec3Helper::pop( ls, 2, v2 );

    Vec3Helper::push( ls, v1/v2 );

    return 1;
}

static int vec3_div( lua_State * ls )
{
    Vec3f v3;
    Vec3i v1, v2;

    Vec3Helper::pop( ls, 1, v1 );
    Vec3Helper::pop( ls, 2, v2 );

    v3.x = (float)v1.x / (float)v2.x;
    v3.y = (float)v1.y / (float)v2.y;
    v3.z = (float)v1.z / (float)v2.z;

    Vec3Helper::push( ls, v3 );

    return 1;
}

static int vec3_new( lua_State * ls )
{
    Vec3i v;

    v.x = lua_tonumber( ls, 1 );
    v.y = lua_tonumber( ls, 2 );
    v.z = lua_tonumber( ls, 3 );

    Vec3Helper::push( ls, v );

    return 1;
}

static int vec3_tostring( lua_State * ls )
{
    Vec3i v;

    if ( Vec3Helper::pop( ls, 1, v ) )
    {
        lua_pushfstring( ls,
                "Vec3 = { x=%d, y=%d, z=%d }", v.x, v.y, v.z );
    }
    else
    {
        Vec3f vf;
        Vec3Helper::pop( ls, 1, vf );

        lua_pushfstring( ls,
                "Vec3 = { x=%f, y=%f, z=%f }", vf.x, vf.y, vf.z );
    }

    return 1;
}

static int vec3_disqr( lua_State * ls )
{
    Vec3i v1, v2;

    Vec3Helper::pop( ls, 1, v1 );
    Vec3Helper::pop( ls, 2, v2 );

    int x = v2.x - v1.x;
    int y = v2.y - v1.y;
    int z = v2.z - v1.z;

    lua_pushinteger( ls, x*x+y*y+z*z );

    return 1;
}

static int vec3_distance( lua_State * ls )
{
    Vec3i v1, v2;

    Vec3Helper::pop( ls, 1, v1 );
    Vec3Helper::pop( ls, 2, v2 );

    lua_pushinteger( ls, v1.distance(v2) );

    return 1;
}

extern "C"
{

LUALIB_API int luaopen_vec3( lua_State * ls )
{
    luaL_Reg meta[] =
    {
        { "__add", vec3_add },
        { "__sub", vec3_sub },
        { "__mul", vec3_mul },
        { "__div", vec3_div },
        { "__idiv", vec3_idiv },
        { "__tostring", vec3_tostring },
        { "disqr", vec3_disqr },
        { "distance", vec3_distance },
        { NULL, NULL }
    };

    luaL_Reg methods[] =
    {
        { "new", vec3_new },
        { "disqr", vec3_disqr },
        { "distance", vec3_distance },
        { NULL, NULL }
    };

    luaL_newmetatable( ls, VEC3META );
    lua_pushvalue( ls, -1 );
    lua_setfield( ls, -2, "__index" );
    luaL_setfuncs( ls, meta, 0 );

    luaL_newlib( ls, methods );

    return 1;
}

}
