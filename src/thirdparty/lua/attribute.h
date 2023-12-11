
#ifndef __SRC_LUA_ATTRIBUTE_H__
#define __SRC_LUA_ATTRIBUTE_H__

#include "base/role.h"
#include "base/battle.h"
#include "lua/kaguya.hpp"

namespace kaguya {

template<> struct lua_type_traits<Attribute<>>
{
    using AttrValue = Attribute<>::Type;
    using AttrValues = std::vector<AttrValue>;

    typedef Attribute<> get_type;
    typedef const Attribute<> & push_type;

    static bool check( lua_State * l, int index )
    {
        LuaStackRef t( l, index );
        optional<AttrValues> values = t.getField<optional<AttrValues>>( "values" );
        return values;
    }

    static bool strictcheck( lua_State * l, int index )
    {
        return check( l, index );
    }

    static get_type get( lua_State * l, int index )
    {
        Attribute<> data;
        LuaStackRef t( l, index );
        data.values = t.getField<AttrValues> ( "values" );
        return data;
    }

    static int push( lua_State * l, push_type v )
    {
        lua_createtable( l, 0, 1 );
        LuaStackRef table( l, -1 );
        table.setRawField( "values", v.values );
        return 1;
    }
};

template<> struct lua_type_traits<Skill>
{
    typedef Skill get_type;
    typedef const Skill & push_type;

    static bool check( lua_State * l, int index )
    {
        LuaStackRef t( l, index );
        optional<SpellID> id = t.getField<optional<SpellID>>( "id" );
        optional<int32_t> type = t.getField<optional<int32_t>>( "type" );
        optional<int32_t> level = t.getField<optional<int32_t>>( "level" );
        optional<uint32_t> building = t.getField<optional<uint32_t>>( "building" );
        return id && level && building;
    }

    static bool strictcheck( lua_State * l, int index )
    {
        return check( l, index );
    }

    static get_type get( lua_State * l, int index )
    {
        Skill data;
        LuaStackRef t( l, index );
        data.id = t.getField<SpellID>( "id" );
        data.type = t.getField<int32_t>( "type" );
        data.level = t.getField<int32_t>( "level" );
        data.building = t.getField<int32_t>( "building" );

        return data;
    }

    static int push( lua_State * l, push_type v )
    {
        lua_createtable( l, 0, 4 );
        LuaStackRef table( l, -1 );
        table.setRawField( "id", v.id );
        table.setRawField( "type", v.type );
        table.setRawField( "level", v.level );
        table.setRawField( "building", v.building );
        return 1;
    }
};

} // namespace kaguya

#endif
