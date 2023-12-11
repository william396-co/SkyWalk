#pragma once

#include "lua/luaenv.h"
#include "protocol/types.h"

namespace google {
namespace protobuf {
    class Descriptor;
    class FieldDescriptor;
}
}; // namespace google

struct pb2lua
{
public :
    // GameMessage to Lua
    static bool push( const GameMessage * msg, lua_State * state );

private :
    static bool do_field( const GameMessage * msg,
            const google::protobuf::FieldDescriptor * field, lua_State * state );
    static bool do_message( const GameMessage * msg,
            const google::protobuf::Descriptor * descriptor, lua_State * state );
    static bool do_single_field( const GameMessage * msg,
            const google::protobuf::FieldDescriptor * field, lua_State * state );
    static bool do_multiple_field( const GameMessage * msg,
            const google::protobuf::FieldDescriptor * field, lua_State * state, int index );

    static bool do_map_field( const GameMessage * msg,
            const google::protobuf::FieldDescriptor * field, lua_State * state );
    static bool do_required_field( const GameMessage * msg,
            const google::protobuf::FieldDescriptor * field, lua_State * state );
    static bool do_optional_field( const GameMessage * msg,
            const google::protobuf::FieldDescriptor * field, lua_State * state );
    static bool do_repeated_field( const GameMessage * msg,
            const google::protobuf::FieldDescriptor * field, lua_State * state );
};

namespace kaguya {

template<> struct lua_type_traits<GameMessage *>
{
    static bool check( lua_State * l, int index ) { return false; }
    static bool strictcheck( lua_State * l, int index ) { return false; }
    static GameMessage * get( lua_State * l, int index ) { return nullptr; }
    static int push( lua_State * l, const GameMessage * v ) { return pb2lua::push( v, l ) ? 1 : 0; }
};

} // namespace kaguya
