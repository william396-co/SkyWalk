
#include "lua/library.h"

#include "fpm/fixed.hpp"
#include "fpm/ios.hpp"
#include "fpm/math.hpp"

#define FIXPOINT_META "fixpoint::meta"
using fixed64 = fpm::fixed<int64_t, int64_t, 16>;

inline static int fixpoint_add( lua_State * s ) {
    if ( lua_gettop(s) != 2 ) {
        luaL_error( s, "Wrong number for add operator" );
    }
    auto p1 = fixed64::from_raw_value( luaL_checkinteger( s, 1 ) );
    auto p2 = fixed64::from_raw_value( luaL_checkinteger( s, 2 ) );
    lua_pushinteger( s, ( p1 + p2 ).raw_value() );
    return 1;
}

inline static int fixpoint_sub( lua_State * s ) {
    if ( lua_gettop(s) != 2 ) {
        luaL_error( s, "Wrong number for sub operator" );
    }
    auto p1 = fixed64::from_raw_value( luaL_checkinteger( s, 1 ) );
    auto p2 = fixed64::from_raw_value( luaL_checkinteger( s, 2 ) );
    lua_pushinteger( s, ( p1 - p2 ).raw_value() );
    return 1;
}

inline static int fixpoint_mul( lua_State * s ) {
    if ( lua_gettop(s) != 2 ) {
        luaL_error( s, "Wrong number for mul operator" );
    }
    auto p1 = fixed64::from_raw_value( luaL_checkinteger( s, 1 ) );
    auto p2 = fixed64::from_raw_value( luaL_checkinteger( s, 2 ) );
    lua_pushinteger( s, ( p1 * p2 ).raw_value() );
    return 1;
}

inline static int fixpoint_div( lua_State * s ) {
    if ( lua_gettop(s) != 2 ) {
        luaL_error( s, "Wrong number for div operator" );
    }
    auto p1 = fixed64::from_raw_value( luaL_checkinteger( s, 1 ) );
    auto p2 = fixed64::from_raw_value( luaL_checkinteger( s, 2 ) );
    lua_pushinteger( s, ( p1 / p2 ).raw_value() );
    return 1;
}

inline static int fixpoint_mod( lua_State * s ) {
    if ( lua_gettop(s) != 2 ) {
        luaL_error( s, "Wrong number for mod operator" );
    }
    auto p1 = fixed64::from_raw_value( luaL_checkinteger( s, 1 ) );
    auto p2 = fixed64::from_raw_value( luaL_checkinteger( s, 2 ) );
    lua_pushinteger( s, fpm::fmod( p1, p2 ).raw_value() );
    return 1;
}

inline static int fixpoint_pow( lua_State * s ) {
    if ( lua_gettop(s) != 2 ) {
        luaL_error( s, "Wrong number for pow operator" );
    }
    auto p1 = fixed64::from_raw_value( luaL_checkinteger( s, 1 ) );
    auto p2 = fixed64::from_raw_value( luaL_checkinteger( s, 2 ) );
    lua_pushinteger( s, fpm::pow( p1, p2 ).raw_value() );
    return 1;
}

inline static int fixpoint_unm( lua_State * s ) {
    if ( lua_gettop(s) != 1 ) {
        luaL_error( s, "Wrong number for unm" );
    }
    auto v = luaL_checkinteger( s, 1 );
    auto fp = fixed64::from_raw_value( v );
    lua_pushinteger( s, ( fp * -1 ).raw_value()  );
    return 1;
}

inline static int fixpoint_eq( lua_State * s ) {
    if ( lua_gettop(s) != 2 ) {
        luaL_error( s, "Wrong number for comparison operator" );
    }
    auto p1 = fixed64::from_raw_value( luaL_checkinteger( s, 1 ) );
    auto p2 = fixed64::from_raw_value( luaL_checkinteger( s, 2 ) );
    lua_pushboolean( s, p1 == p2 );
    return 1;
}

inline static int fixpoint_lt( lua_State * s ) {
    if ( lua_gettop(s) != 2 ) {
        luaL_error( s, "Wrong number for comparison operator" );
    }
    auto p1 = fixed64::from_raw_value( luaL_checkinteger( s, 1 ) );
    auto p2 = fixed64::from_raw_value( luaL_checkinteger( s, 2 ) );
    lua_pushboolean( s, p1 < p2 );
    return 1;
}

inline static int fixpoint_le( lua_State * s ) {
    if ( lua_gettop(s) != 2 ) {
        luaL_error( s, "Wrong number for comparison operator" );
    }
    auto p1 = fixed64::from_raw_value( luaL_checkinteger( s, 1 ) );
    auto p2 = fixed64::from_raw_value( luaL_checkinteger( s, 2 ) );
    lua_pushboolean( s, p1 <= p2 );
    return 1;
}

inline static int fixpoint_create( lua_State * s ) {
    auto v = luaL_checknumber( s, 1 );
    lua_pushinteger( s, fixed64( v ).raw_value() );
    return 1;
}

inline static int fixpoint_tonumber( lua_State * s ) {
    if ( lua_gettop(s) != 1 ) {
        luaL_error( s, "Wrong number for fixpoint-to-number" );
    }
    auto v = luaL_checkinteger( s, 1 );
    auto fp = fixed64::from_raw_value( v );
    lua_pushnumber( s, (double)fp );
    return 1;
}

inline static int fixpoint_tostring( lua_State * s ) {
    if ( lua_gettop(s) != 1 ) {
        luaL_error( s, "Wrong number for fixpoint-to-string" );
    }
    auto v = luaL_checkinteger( s, 1 );
    auto fp = fixed64::from_raw_value( v );
    lua_pushfstring( s, "%f", (double)fp );
    return 1;
}

inline static int fixpoint_tointeger( lua_State * s ) {
    if ( lua_gettop(s) != 1 ) {
        luaL_error( s, "Wrong number for fixpoint-to-integer" );
    }
    auto v = luaL_checkinteger( s, 1 );
    auto fp = fixed64::from_raw_value( v );
    lua_pushinteger( s, (int64_t)fp );
    return 1;
}

inline static int fixpoint_ceil( lua_State * s ) {
    if ( lua_gettop(s) != 1 ) {
        luaL_error( s, "Wrong number for ceil" );
    }
    auto v = luaL_checkinteger( s, 1 );
    auto fp = fixed64::from_raw_value( v );
    lua_pushinteger( s, fpm::ceil( fp ).raw_value() );
    return 1;
}

inline static int fixpoint_floor( lua_State * s ) {
    if ( lua_gettop(s) != 1 ) {
        luaL_error( s, "Wrong number for floor" );
    }
    auto v = luaL_checkinteger( s, 1 );
    auto fp = fixed64::from_raw_value( v );
    lua_pushinteger( s, fpm::floor( fp ).raw_value() );
    return 1;
}

inline static int fixpoint_near( lua_State * s ) {
    if ( lua_gettop(s) != 1 ) {
        luaL_error( s, "Wrong number for near" );
    }
    auto v = luaL_checkinteger( s, 1 );
    auto fp = fixed64::from_raw_value( v );
    lua_pushinteger( s, fpm::nearbyint( fp ).raw_value() );
    return 1;
}

inline static int fixpoint_round( lua_State * s ) {
    if ( lua_gettop(s) != 1 ) {
        luaL_error( s, "Wrong number for round" );
    }
    auto v = luaL_checkinteger( s, 1 );
    auto fp = fixed64::from_raw_value( v );
    lua_pushinteger( s, fpm::round( fp ).raw_value() );
    return 1;
}

inline static int fixpoint_abs( lua_State * s ) {
    if ( lua_gettop(s) != 1 ) {
        luaL_error( s, "Wrong number for abs" );
    }
    auto v = luaL_checkinteger( s, 1 );
    auto fp = fixed64::from_raw_value( v );
    lua_pushinteger( s, fpm::abs( fp ).raw_value() );
    return 1;
}

inline static int fixpoint_min( lua_State * s ) {
    if ( lua_gettop(s) != 2 ) {
        luaL_error( s, "Wrong number for min" );
    }
    auto a = luaL_checkinteger( s, 1 );
    auto b = luaL_checkinteger( s, 2 );
    lua_pushinteger( s, fixed64::from_raw_value( std::min( a, b ) ).raw_value() );
    return 1;
}

inline static int fixpoint_max( lua_State * s ) {
    if ( lua_gettop(s) != 2 ) {
        luaL_error( s, "Wrong number for min" );
    }
    auto a = luaL_checkinteger( s, 1 );
    auto b = luaL_checkinteger( s, 2 );
    lua_pushinteger( s, fixed64::from_raw_value( std::max( a, b ) ).raw_value() );
    return 1;
}

inline static int fixpoint_sqrt( lua_State * s ) {
    if ( lua_gettop(s) != 1 ) {
        luaL_error( s, "Wrong number for sqrt" );
    }
    auto v = luaL_checkinteger( s, 1 );
    auto fp = fixed64::from_raw_value( v );
    lua_pushinteger( s, fpm::sqrt( fp ).raw_value() );
    return 1;
}

inline static int fixpoint_sin( lua_State * s ) {
    if ( lua_gettop(s) != 1 ) {
        luaL_error( s, "Wrong number for sin" );
    }
    auto v = luaL_checkinteger( s, 1 );
    auto fp = fixed64::from_raw_value( v );
    lua_pushinteger( s, fpm::sin( fp ).raw_value() );
    return 1;
}

inline static int fixpoint_cos( lua_State * s ) {
    if ( lua_gettop(s) != 1 ) {
        luaL_error( s, "Wrong number for cos" );
    }
    auto v = luaL_checkinteger( s, 1 );
    auto fp = fixed64::from_raw_value( v );
    lua_pushinteger( s, fpm::cos( fp ).raw_value() );
    return 1;
}

inline static int fixpoint_tan( lua_State * s ) {
    if ( lua_gettop(s) != 1 ) {
        luaL_error( s, "Wrong number for tan" );
    }
    auto v = luaL_checkinteger( s, 1 );
    auto fp = fixed64::from_raw_value( v );
    lua_pushinteger( s, fpm::tan( fp ).raw_value() );
    return 1;
}

inline static int fixpoint_asin( lua_State * s ) {
    if ( lua_gettop(s) != 1 ) {
        luaL_error( s, "Wrong number for asin" );
    }
    auto v = luaL_checkinteger( s, 1 );
    auto fp = fixed64::from_raw_value( v );
    lua_pushinteger( s, fpm::asin( fp ).raw_value() );
    return 1;
}

inline static int fixpoint_acos( lua_State * s ) {
    if ( lua_gettop(s) != 1 ) {
        luaL_error( s, "Wrong number for acos" );
    }
    auto v = luaL_checkinteger( s, 1 );
    auto fp = fixed64::from_raw_value( v );
    lua_pushinteger( s, fpm::acos( fp ).raw_value() );
    return 1;
}

inline static int fixpoint_atan( lua_State * s ) {
    if ( lua_gettop(s) != 1 ) {
        luaL_error( s, "Wrong number for atan" );
    }
    auto v = luaL_checkinteger( s, 1 );
    auto fp = fixed64::from_raw_value( v );
    lua_pushinteger( s, fpm::atan( fp ).raw_value() );
    return 1;
}

#ifdef __cplusplus
extern "C" {
#endif

LUA_API int luaopen_fixpoint( lua_State * s ) {
    luaL_Reg meta[] = {
        { "__add", fixpoint_add },
        { "__sub", fixpoint_sub },
        { "__mul", fixpoint_mul },
        { "__div", fixpoint_div },
        { "__mod", fixpoint_mod },
        { "__unm", fixpoint_unm },
        { "__pow", fixpoint_pow },
        { "__tostring", fixpoint_tostring },
        { "__eq", fixpoint_eq },
        { "__lt", fixpoint_lt },
        { "__le", fixpoint_le },
        { nullptr, nullptr }
    };

    luaL_Reg methods[] = {
        { "new", fixpoint_create },
        { "tonumber", fixpoint_tonumber },
        { "tostring", fixpoint_tostring },
        { "tointeger", fixpoint_tointeger },
        { "ceil", fixpoint_ceil },
        { "floor", fixpoint_floor },
        { "near", fixpoint_near },
        { "round", fixpoint_round },
        { "abs", fixpoint_abs },
        { "min", fixpoint_min },
        { "max", fixpoint_max },
        { "sqrt", fixpoint_sqrt },
        { "sin", fixpoint_sin },
        { "cos", fixpoint_cos },
        { "tan", fixpoint_tan },
        { "asin", fixpoint_asin },
        { "acos", fixpoint_acos },
        { "atan", fixpoint_atan },
        { "add", fixpoint_add },
        { "sub", fixpoint_sub },
        { "mul", fixpoint_mul },
        { "div", fixpoint_div },
        { "mod", fixpoint_mod },
        { "unm", fixpoint_unm },
        { "pow", fixpoint_pow },
        { nullptr, nullptr }
    };

    luaL_newmetatable( s, FIXPOINT_META );
    lua_pushvalue( s, -1 );
    lua_setfield( s, -2, "__index" );
    luaL_setfuncs( s, meta, 0 );
    luaL_newlib( s, methods );

    return 1;
}

#ifdef __cplusplus
}
#endif
