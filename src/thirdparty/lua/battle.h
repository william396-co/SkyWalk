
#ifndef __SRC_LUA_BATTLE_H__
#define __SRC_LUA_BATTLE_H__

#include "base/battle.h"
#include "lua/kaguya.hpp"

namespace kaguya {

template <> struct lua_type_traits<BattleOrder> {
    typedef BattleOrder get_type;
    typedef const BattleOrder & push_type;

    static bool check( lua_State * l, int index )
    {
        LuaStackRef t( l, index );
        optional<UnitID> roleid = t.getField<optional<UnitID>>( "roleid" );
        optional<HeroID> heroid = t.getField<optional<HeroID>>( "heroid" );
        optional<int32_t> action = t.getField<optional<int32_t>>( "action" );
        optional<Int64Vec> values = t.getField<optional<Int64Vec>>( "values" );
        return heroid && roleid && action && values ;
    }

    static bool strictcheck( lua_State * l, int index )
    {
        return check( l, index );
    }

    static get_type get( lua_State * l, int index )
    {
        BattleOrder data;
        LuaStackRef t( l, index );
        data.roleid = t.getField<UnitID>( "roleid" );
        data.heroid = t.getField<HeroID>( "heroid" );
        data.action = t.getField<int32_t>( "action" );
        data.values = t.getField<Int64Vec>( "values" );
        return data;
    }

    static int push( lua_State * l, push_type v )
    {
        lua_createtable( l, 0, 4 );
        LuaStackRef table( l, -1 );
        table.setRawField( "roleid", v.roleid );
        table.setRawField( "heroid", v.heroid );
        table.setRawField( "action", v.action );
        table.setRawField( "values", v.values );
        return 1;
    }
};

template <> struct lua_type_traits<BattleFrame> {
    typedef BattleFrame get_type;
    typedef const BattleFrame & push_type;

    static bool check( lua_State * l, int index )
    {
        LuaStackRef t( l, index );
        optional<int32_t> frame = t.getField<optional<int32_t>>( "frame" );
        optional<BattleOrders> orders = t.getField<optional<BattleOrders>>( "orderlist" );
        return frame && orders;
    }

    static bool strictcheck( lua_State * l, int index )
    {
        return check( l, index );
    }

    static get_type get( lua_State * l, int index )
    {
        BattleFrame data;
        LuaStackRef t( l, index );
        data.frame = t.getField<>( "frame" );
        data.orderlist = t.getField<BattleOrders>( "orderlist" );
        return data;
    }

    static int push( lua_State * l, push_type v )
    {
        lua_createtable( l, 0, 2 );
        LuaStackRef table( l, -1 );
        table.setRawField( "frame", v.frame );
        table.setRawField( "orderlist", v.orderlist );
        return 1;
    }
};

} // namespace kaguya

#endif
