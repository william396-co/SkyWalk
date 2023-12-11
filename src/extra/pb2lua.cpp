
#include <google/protobuf/message.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/reflection.h>

#include "pb2lua.h"

bool pb2lua::push( const GameMessage * msg, lua_State * state )
{
    auto descriptor = msg->GetDescriptor();
    if ( descriptor == nullptr ) {
        return false;
    }

    return do_message( msg, descriptor, state );
}

bool pb2lua::do_field( const GameMessage * msg, const google::protobuf::FieldDescriptor * field, lua_State * state )
{
    if ( field->is_map() ) {
        return do_map_field( msg, field, state );
    } else if ( field->is_required() ) {
        return do_required_field( msg, field, state );
    } else if ( field->is_optional() ) {
        return do_optional_field( msg, field, state );
    } else if ( field->is_repeated() ) {
        return do_repeated_field( msg, field, state );
    }

    return false;
}

bool pb2lua::do_message( const GameMessage * msg, const google::protobuf::Descriptor * descriptor, lua_State * state )
{
    int field_count = descriptor->field_count();

    lua_createtable( state, 0, field_count );
    for ( int i = 0; i < field_count; ++i ) {
        auto field = descriptor->field( i );
        if ( !do_field( msg, field, state ) ) {
            return false;
        }
        lua_setfield( state, -2, field->name().c_str() );
    }

    return true;
}

bool pb2lua::do_single_field( const GameMessage * msg, const google::protobuf::FieldDescriptor * field, lua_State * state )
{
    auto reflection = msg->GetReflection();
    if ( reflection == nullptr ) {
        return false;
    }

    switch ( field->cpp_type() ) {
        case google::protobuf::FieldDescriptor::CPPTYPE_DOUBLE:
            lua_pushnumber( state, reflection->GetDouble( *msg, field ) );
            break;
        case google::protobuf::FieldDescriptor::CPPTYPE_FLOAT:
            lua_pushnumber( state, reflection->GetFloat( *msg, field ) );
            break;
        case google::protobuf::FieldDescriptor::CPPTYPE_INT32:
            lua_pushinteger( state, reflection->GetInt32( *msg, field ) );
            break;
        case google::protobuf::FieldDescriptor::CPPTYPE_UINT32:
            lua_pushinteger( state, reflection->GetUInt32( *msg, field ) );
            break;
        case google::protobuf::FieldDescriptor::CPPTYPE_INT64:
            lua_pushinteger( state, reflection->GetInt64( *msg, field ) );
            break;
        case google::protobuf::FieldDescriptor::CPPTYPE_UINT64:
            lua_pushinteger( state, reflection->GetUInt64( *msg, field ) );
            break;
        case google::protobuf::FieldDescriptor::CPPTYPE_ENUM:
            lua_pushinteger( state, reflection->GetEnumValue( *msg, field ) );
            break;
        case google::protobuf::FieldDescriptor::CPPTYPE_BOOL:
            lua_pushboolean( state, reflection->GetBool( *msg, field ) );
            break;
        case google::protobuf::FieldDescriptor::CPPTYPE_STRING: {
            std::string value = reflection->GetString( *msg, field );
            lua_pushlstring( state, value.c_str(), value.size() );
        } break;
        case google::protobuf::FieldDescriptor::CPPTYPE_MESSAGE: {
            auto & submsg = reflection->GetMessage( *msg, field );
            return do_message( &submsg, field->message_type(), state );
        } break;
        default:
            return false;
            break;
    }

    return true;
}

bool pb2lua::do_multiple_field( const GameMessage * msg, const google::protobuf::FieldDescriptor * field, lua_State * state, int index )
{
    auto reflection = msg->GetReflection();
    if ( reflection == nullptr ) {
        return false;
    }

    switch ( field->cpp_type() ) {
        case google::protobuf::FieldDescriptor::CPPTYPE_DOUBLE:
            lua_pushnumber( state, reflection->GetRepeatedDouble( *msg, field, index ) );
            break;
        case google::protobuf::FieldDescriptor::CPPTYPE_FLOAT:
            lua_pushnumber( state, reflection->GetRepeatedFloat( *msg, field, index ) );
            break;
        case google::protobuf::FieldDescriptor::CPPTYPE_INT32:
            lua_pushinteger( state, reflection->GetRepeatedInt32( *msg, field, index ) );
            break;
        case google::protobuf::FieldDescriptor::CPPTYPE_UINT32:
            lua_pushinteger( state, reflection->GetRepeatedUInt32( *msg, field, index ) );
            break;
        case google::protobuf::FieldDescriptor::CPPTYPE_INT64:
            lua_pushinteger( state, reflection->GetRepeatedInt64( *msg, field, index ) );
            break;
        case google::protobuf::FieldDescriptor::CPPTYPE_UINT64:
            lua_pushinteger( state, reflection->GetRepeatedUInt64( *msg, field, index ) );
            break;
        case google::protobuf::FieldDescriptor::CPPTYPE_ENUM:
            lua_pushinteger( state, reflection->GetRepeatedEnumValue( *msg, field, index ) );
            break;
        case google::protobuf::FieldDescriptor::CPPTYPE_BOOL:
            lua_pushboolean( state, reflection->GetRepeatedBool( *msg, field, index ) );
            break;
        case google::protobuf::FieldDescriptor::CPPTYPE_STRING: {
            std::string value = reflection->GetRepeatedString( *msg, field, index );
            lua_pushlstring( state, value.c_str(), value.size() );
        } break;
        case google::protobuf::FieldDescriptor::CPPTYPE_MESSAGE: {
            auto & submsg = reflection->GetRepeatedMessage( *msg, field, index );
            return do_message( &submsg, field->message_type(), state );
        } break;
        default:
            return false;
            break;
    }

    return true;
}

bool pb2lua::do_map_field( const GameMessage * msg, const google::protobuf::FieldDescriptor * field, lua_State * state )
{
    auto reflection = msg->GetReflection();
    auto descriptor = field->message_type();
    if ( reflection == nullptr
        || descriptor == nullptr ) {
        return false;
    }

    assert( descriptor->field_count() == 2 );
    auto key = descriptor->field( 0 );
    auto value = descriptor->field( 1 );

    int field_size = reflection->FieldSize( *msg, field );
    lua_createtable( state, 0, field_size );
    for ( int i = 0; i < field_size; ++i ) {
        const GameMessage & submsg = reflection->GetRepeatedMessage( *msg, field, i );
        if ( !do_field( &submsg, key, state )
            || !do_field( &submsg, value, state ) ) {
            return false;
        }
        lua_settable( state, -3 );
    }

    return true;
}

bool pb2lua::do_required_field( const GameMessage * msg, const google::protobuf::FieldDescriptor * field, lua_State * state )
{
    auto reflection = msg->GetReflection();
    if ( reflection == nullptr ) {
        return false;
    }

    if ( !reflection->HasField( *msg, field ) ) {
        return false;
    }

    return do_single_field( msg, field, state );
}

bool pb2lua::do_optional_field( const GameMessage * msg, const google::protobuf::FieldDescriptor * field, lua_State * state )
{
    auto reflection = msg->GetReflection();
    if ( reflection == nullptr ) {
        return false;
    }

    if ( !reflection->HasField( *msg, field )
        && field->cpp_type() == google::protobuf::FieldDescriptor::CPPTYPE_MESSAGE ) {
        lua_pushnil( state );
        return true;
    }

    return do_single_field( msg, field, state );
}

bool pb2lua::do_repeated_field( const GameMessage * msg, const google::protobuf::FieldDescriptor * field, lua_State * state )
{
    auto reflection = msg->GetReflection();
    if ( reflection == nullptr ) {
        return false;
    }

    int field_count = reflection->FieldSize( *msg, field );

    lua_createtable( state, field_count, 0 );
    for ( int i = 0; i < field_count; ++i ) {
        if ( !do_multiple_field( msg, field, state, i ) ) {
            return false;
        }
        lua_seti( state, -2, i+1 );
    }

    return true;
}
