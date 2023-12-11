
#ifndef __SRC_LUA_LOCATION_H__
#define __SRC_LUA_LOCATION_H__

#include "lua/kaguya.hpp"
#include "domain/location.h"

namespace kaguya {

template<> struct lua_type_traits<Vec2i>
{
    typedef Vec2i get_type;
    typedef const Vec2i & push_type;

    static bool check( lua_State * l, int index )
    {
        LuaStackRef t( l, index );
        optional<int32_t> x = t.getField<optional<int32_t>>( "x" );
        optional<int32_t> y = t.getField<optional<int32_t>>( "y" );
        return x && y;
    }

    static bool strictcheck( lua_State * l, int index )
    {
        return check( l, index );
    }

    static get_type get( lua_State * l, int index )
    {
        Vec2i data;
        LuaStackRef t( l, index );

        data.x = t.getField<int32_t>( "x" );
        data.y = t.getField<int32_t>( "y" );

        return data;
    }

    static int push( lua_State * l, push_type v )
    {
        lua_createtable( l, 0, 2 );
        LuaStackRef table( l, -1 );
        table.setRawField( "x", v.x );
        table.setRawField( "y", v.y );
        return 1;
    }
};

template<> struct lua_type_traits<Location>
{
    typedef Location get_type;
    typedef const Location & push_type;

    static bool check( lua_State * l, int index )
    {
        LuaStackRef t( l, index );
        optional<Vec2i> pos = t.getField<optional<Vec2i>>( "pos" );
        optional<int32_t> movement = t.getField<optional<int32_t>>( "movement" );
        optional<int32_t> direction = t.getField<optional<int32_t>>( "direction" );
        return pos && movement && direction;
    }

    static bool strictcheck( lua_State * l, int index )
    {
        return check( l, index );
    }

    static get_type get( lua_State * l, int index )
    {
        Location data;
        LuaStackRef t( l, index );

        data.pos = t.getField<Vec2i>( "pos" );
        data.movement = t.getField<int32_t>( "movement" );
        data.direction = t.getField<int32_t>( "direction" );

        return data;
    }

    static int push( lua_State * l, push_type v )
    {
        lua_createtable( l, 0, 3 );
        LuaStackRef table( l, -1 );
        table.setRawField( "pos", v.pos );
        table.setRawField( "movement", v.movement );
        table.setRawField( "direction", v.direction );
        return 1;
    }
};

} // namespace kaguya

#endif
