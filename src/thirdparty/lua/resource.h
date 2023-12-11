
#ifndef __SRC_LUA_RESOURCE_H__
#define __SRC_LUA_RESOURCE_H__

#include "lua/kaguya.hpp"
#include "base/resource.h"

namespace kaguya {

template <> struct lua_type_traits<Resource> {
    typedef Resource get_type;
    typedef const Resource & push_type;

    static bool check( lua_State * l, int index ) {
        LuaStackRef t( l, index );
        optional<ResourceType> type = t.getField<optional<ResourceType>>( "type" );
        optional<uint32_t> object = t.getField<optional<uint32_t>>( "object" );
        optional<int32_t> count = t.getField<optional<int32_t>>( "count" );
        optional<int32_t> param1 = t.getField<optional<int32_t>>( "param1" );
        optional<int32_t> param2 = t.getField<optional<int32_t>>( "param2" );
        return type && object && count;
    }

    static bool strictcheck( lua_State * l, int index ) {
        return check( l, index );
    }

    static get_type get( lua_State * l, int index ) {
        Resource data;
        LuaStackRef t( l, index );
        data.type = t.getField<ResourceType>( "type" );
        data.object = t.getField<uint32_t>( "object" );
        data.count = t.getField<int32_t>( "count" );
        data.param1 = t.getField<optional<int32_t>>( "param1" ).value_or( -1 );
        data.param2 = t.getField<optional<int32_t>>( "param2" ).value_or( -1 );
        return data;
    }

    static int push( lua_State * l, push_type v ) {
        lua_createtable( l, 0, 5 );
        LuaStackRef table( l, -1 );
        table.setRawField( "type", v.type );
        table.setRawField( "object", v.object );
        table.setRawField( "count", v.count );
        table.setRawField( "param1", v.param1 );
        table.setRawField( "param2", v.param2 );
        return 1;
    }
};

} // namespace kaguya

#endif
