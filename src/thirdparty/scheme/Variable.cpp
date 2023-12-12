
#include <cstdio>
#include <cassert>
#include <cinttypes>
#include <cstdlib>

#include "lua/kaguya.hpp"
#include "utils/integer.h"
#include "utils/streambuf.h"

#include "Variable.hpp"

namespace data
{

// Tablename
const std::string Variable_Data::TABLENAME = "Variable";

// VARCHAR Length Limit

Variable_Data::Variable_Data()
{
    clean();
    // Fields
    m_db_id = 0;
    m_db_roleid = 0;
    m_db_mailid = 0;
    m_db_teamid = 0;
    m_db_entityid = 0;
    m_db_imageid = 0;
    m_db_guildid = 0;
    m_db_heroid = 0;
    m_db_logid = 0;
    m_db_messageid = 0;
}

Variable_Data::Variable_Data( int16_t id )
{
    clean();
    // Fields
    m_db_id = id;
    m_db_roleid = 0;
    m_db_mailid = 0;
    m_db_teamid = 0;
    m_db_entityid = 0;
    m_db_imageid = 0;
    m_db_guildid = 0;
    m_db_heroid = 0;
    m_db_logid = 0;
    m_db_messageid = 0;
}

Variable_Data::~Variable_Data()
{
    m_db_id = 0;
    m_db_roleid = 0;
    m_db_mailid = 0;
    m_db_teamid = 0;
    m_db_entityid = 0;
    m_db_imageid = 0;
    m_db_guildid = 0;
    m_db_heroid = 0;
    m_db_logid = 0;
    m_db_messageid = 0;
}

void Variable_Data::clean()
{
    m_method = DBMethod::None;
    bzero( &m_dirty, sizeof(m_dirty) );
}

ISQLData * Variable_Data::clone( bool isfull ) const
{
    Variable_Data * data = new Variable_Data();

    if ( data != nullptr && isfull )
    {
        data->m_method = m_method;
        memcpy( data->m_dirty, m_dirty, NFIELDS );

        data->m_db_id = m_db_id;
        data->m_db_roleid = m_db_roleid;
        data->m_db_mailid = m_db_mailid;
        data->m_db_teamid = m_db_teamid;
        data->m_db_entityid = m_db_entityid;
        data->m_db_imageid = m_db_imageid;
        data->m_db_guildid = m_db_guildid;
        data->m_db_heroid = m_db_heroid;
        data->m_db_logid = m_db_logid;
        data->m_db_messageid = m_db_messageid;
    }

    return data;
}

bool Variable_Data::isDirty() const
{
    for ( uint16_t i = 0; i < NFIELDS; ++i )
    {
        if ( m_dirty[i] != 0 ) return true;
    }

    return false;
}

bool Variable_Data::isKeydirty() const
{
    return m_dirty[0] == 1;
}

void Variable_Data::keystring( std::string & k ) const
{
    k = utils::Integer::toString( m_db_id );
}

void Variable_Data::indexstring( uint8_t op, std::string & k ) const
{
    if ( likely(op == eCodec_All) )
    {
        k = utils::Integer::toString( m_db_id );
    }
    else
    {
        if ( m_dirty[0] == 1 )
        {
            if ( !k.empty() ) k += "#";
            k += utils::Integer::toString( m_db_id );
        }
    }
}

void Variable_Data::autoincrease( uint8_t source, const Slice & value )
{
}

bool Variable_Data::query( std::string & sqlcmd, std::vector<std::string> & escapevalues, bool all )
{
    std::string fields;

    for ( uint16_t i = 0; i < NFIELDS; ++i )
    {
        if ( m_dirty[i] == 0 )
        {
            continue;
        }

        if ( !fields.empty() )
        {
            fields += " AND ";
        }

        switch ( i )
        {
            case 0 :
                fields += "`id`=";
                fields += utils::Integer::toString( m_db_id );
                break;
        }
    }

    if ( fields.empty() )
    {
        sqlcmd = "SELECT `id`,`roleid`,`mailid`,`teamid`,`entityid`,`imageid`,`guildid`,`heroid`,`logid`,`messageid` FROM `Variable`";
    }
    else
    {
        uint32_t len = 32 + 91 + 8 + 8 + fields.size();
        sqlcmd.resize( len );
        len = std::snprintf(
            (char *)sqlcmd.data(), len-1,
            "SELECT `id`,`roleid`,`mailid`,`teamid`,`entityid`,`imageid`,`guildid`,`heroid`,`logid`,`messageid` FROM `Variable` WHERE %s LIMIT 1", fields.c_str() );
        sqlcmd.resize( len );
    }

    return true;
}

bool Variable_Data::insert( std::string & sqlcmd, std::vector<std::string> & escapevalues )
{
    std::string fields, values;

    for ( uint16_t i = 0; i < NFIELDS; ++i )
    {
        if ( m_dirty[i] == 0 && i != 0 )
        {
            continue;
        }

        if ( !fields.empty() )
        {
            fields += ", ";
        }
        if ( !values.empty() )
        {
            values += ", ";
        }

        switch ( i )
        {
            case 0 :
                fields += "`id`";
                values += utils::Integer::toString( m_db_id );
                break;
            case 1 :
                fields += "`roleid`";
                values += utils::Integer::toString( m_db_roleid );
                break;
            case 2 :
                fields += "`mailid`";
                values += utils::Integer::toString( m_db_mailid );
                break;
            case 3 :
                fields += "`teamid`";
                values += utils::Integer::toString( m_db_teamid );
                break;
            case 4 :
                fields += "`entityid`";
                values += utils::Integer::toString( m_db_entityid );
                break;
            case 5 :
                fields += "`imageid`";
                values += utils::Integer::toString( m_db_imageid );
                break;
            case 6 :
                fields += "`guildid`";
                values += utils::Integer::toString( m_db_guildid );
                break;
            case 7 :
                fields += "`heroid`";
                values += utils::Integer::toString( m_db_heroid );
                break;
            case 8 :
                fields += "`logid`";
                values += utils::Integer::toString( m_db_logid );
                break;
            case 9 :
                fields += "`messageid`";
                values += utils::Integer::toString( m_db_messageid );
                break;
        }
    }

    uint32_t len = 32 + 8 + fields.size() + values.size();

    sqlcmd.resize( len );
    len = std::snprintf( (char *)sqlcmd.data(), len-1,
        "INSERT INTO `Variable` (%s) VALUES (%s)", fields.c_str(), values.c_str() );
    sqlcmd.resize(len);

    return true;
}

bool Variable_Data::update( std::string & sqlcmd, std::vector<std::string> & escapevalues )
{
    std::string dirty;

    for ( uint16_t i = 0; i < NFIELDS; ++i )
    {
        if ( m_dirty[i] == 0 )
        {
            continue;
        }

        if ( !dirty.empty() )
        {
            dirty += ", ";
        }

        switch ( i )
        {
            case 0 :
                dirty += "`id`=";
                dirty += utils::Integer::toString( m_db_id );
                break;
            case 1 :
                dirty += "`roleid`=";
                dirty += utils::Integer::toString( m_db_roleid );
                break;
            case 2 :
                dirty += "`mailid`=";
                dirty += utils::Integer::toString( m_db_mailid );
                break;
            case 3 :
                dirty += "`teamid`=";
                dirty += utils::Integer::toString( m_db_teamid );
                break;
            case 4 :
                dirty += "`entityid`=";
                dirty += utils::Integer::toString( m_db_entityid );
                break;
            case 5 :
                dirty += "`imageid`=";
                dirty += utils::Integer::toString( m_db_imageid );
                break;
            case 6 :
                dirty += "`guildid`=";
                dirty += utils::Integer::toString( m_db_guildid );
                break;
            case 7 :
                dirty += "`heroid`=";
                dirty += utils::Integer::toString( m_db_heroid );
                break;
            case 8 :
                dirty += "`logid`=";
                dirty += utils::Integer::toString( m_db_logid );
                break;
            case 9 :
                dirty += "`messageid`=";
                dirty += utils::Integer::toString( m_db_messageid );
                break;
        }
    }

    int32_t len = 32 + 8 + dirty.size() + 7 + 32;

    sqlcmd.resize( len );
    len = std::snprintf( (char *)sqlcmd.data(), len-1,
        "UPDATE `Variable` SET %s WHERE `id`=%d", dirty.c_str(), m_db_id );
    sqlcmd.resize( len );

    return true;
}

bool Variable_Data::remove( std::string & sqlcmd, std::vector<std::string> & escapevalues )
{
    uint32_t len = 32 + 8 + 7 + 32;

    sqlcmd.resize( len );
    len = std::snprintf( (char *)sqlcmd.data(), len-1,
        "DELETE FROM `Variable` WHERE `id`=%d", m_db_id );
    sqlcmd.resize( len );

    return true;
}

bool Variable_Data::replace( std::string & sqlcmd, std::vector<std::string> & escapevalues )
{
    std::string fields, values, dirty;

    for ( uint16_t i = 0; i < NFIELDS; ++i )
    {
        if ( m_dirty[i] == 0 && i != 0 )
        {
            continue;
        }

        if ( !fields.empty() )
        {
            fields += ", ";
        }
        if ( !values.empty() )
        {
            values += ", ";
        }

        switch ( i )
        {
            case 0 :
                fields += "`id`";
                values += utils::Integer::toString( m_db_id );
                break;
            case 1 :
                fields += "`roleid`";
                values += utils::Integer::toString( m_db_roleid );
                break;
            case 2 :
                fields += "`mailid`";
                values += utils::Integer::toString( m_db_mailid );
                break;
            case 3 :
                fields += "`teamid`";
                values += utils::Integer::toString( m_db_teamid );
                break;
            case 4 :
                fields += "`entityid`";
                values += utils::Integer::toString( m_db_entityid );
                break;
            case 5 :
                fields += "`imageid`";
                values += utils::Integer::toString( m_db_imageid );
                break;
            case 6 :
                fields += "`guildid`";
                values += utils::Integer::toString( m_db_guildid );
                break;
            case 7 :
                fields += "`heroid`";
                values += utils::Integer::toString( m_db_heroid );
                break;
            case 8 :
                fields += "`logid`";
                values += utils::Integer::toString( m_db_logid );
                break;
            case 9 :
                fields += "`messageid`";
                values += utils::Integer::toString( m_db_messageid );
                break;
        }
    }
    for ( uint16_t i = 0; i < NFIELDS; ++i )
    {
        if ( m_dirty[i] == 1 && i != 0 )
        {
            if ( !dirty.empty() )
            {
                dirty += ", ";
            }

            switch ( i )
            {
                case 1 :
                    dirty += "`roleid`=";
                    dirty += utils::Integer::toString( m_db_roleid );
                    break;
                case 2 :
                    dirty += "`mailid`=";
                    dirty += utils::Integer::toString( m_db_mailid );
                    break;
                case 3 :
                    dirty += "`teamid`=";
                    dirty += utils::Integer::toString( m_db_teamid );
                    break;
                case 4 :
                    dirty += "`entityid`=";
                    dirty += utils::Integer::toString( m_db_entityid );
                    break;
                case 5 :
                    dirty += "`imageid`=";
                    dirty += utils::Integer::toString( m_db_imageid );
                    break;
                case 6 :
                    dirty += "`guildid`=";
                    dirty += utils::Integer::toString( m_db_guildid );
                    break;
                case 7 :
                    dirty += "`heroid`=";
                    dirty += utils::Integer::toString( m_db_heroid );
                    break;
                case 8 :
                    dirty += "`logid`=";
                    dirty += utils::Integer::toString( m_db_logid );
                    break;
                case 9 :
                    dirty += "`messageid`=";
                    dirty += utils::Integer::toString( m_db_messageid );
                    break;
            }
        }
    }

    uint32_t len = 64 + 8 + fields.size() + values.size() + dirty.size();

    sqlcmd.resize( len );
    len = std::snprintf( (char *)sqlcmd.data(), len-1,
        "INSERT INTO `Variable` (%s) VALUES (%s) ON DUPLICATE KEY UPDATE %s", fields.c_str(), values.c_str(), dirty.c_str() );
    sqlcmd.resize(len);

    return true;
}

Slice Variable_Data::encode( uint8_t op )
{
    uint16_t nfields = 0;
    StreamBuf pack( 1024, sizeof(uint16_t) );

    // fields
    if ( unlikely(op == eCodec_Query) )
    {
        if ( m_dirty[0] == 1 )
        {
            ++nfields;
            pack.encode( (uint16_t)0 );
            pack.encode( m_db_id );
        }
    }
    else
    {
        for ( uint16_t i = 0; i < NFIELDS; ++i )
        {
            if ( op != eCodec_All && m_dirty[i] == 0 )
            {
                continue;
            }

            switch ( i )
            {
                case 0 :
                    ++nfields;
                    pack.encode( i );
                    pack.encode( m_db_id );
                    break;
                case 1 :
                    ++nfields;
                    pack.encode( i );
                    pack.encode( m_db_roleid );
                    break;
                case 2 :
                    ++nfields;
                    pack.encode( i );
                    pack.encode( m_db_mailid );
                    break;
                case 3 :
                    ++nfields;
                    pack.encode( i );
                    pack.encode( m_db_teamid );
                    break;
                case 4 :
                    ++nfields;
                    pack.encode( i );
                    pack.encode( m_db_entityid );
                    break;
                case 5 :
                    ++nfields;
                    pack.encode( i );
                    pack.encode( m_db_imageid );
                    break;
                case 6 :
                    ++nfields;
                    pack.encode( i );
                    pack.encode( m_db_guildid );
                    break;
                case 7 :
                    ++nfields;
                    pack.encode( i );
                    pack.encode( m_db_heroid );
                    break;
                case 8 :
                    ++nfields;
                    pack.encode( i );
                    pack.encode( m_db_logid );
                    break;
                case 9 :
                    ++nfields;
                    pack.encode( i );
                    pack.encode( m_db_messageid );
                    break;
            }
        }
    }

    // key fields
    if ( op != eCodec_Query )
    {
        pack.encode( m_db_id );
    }

    // fields number
    pack.reset();
    pack.encode( nfields );

    return pack.slice();
}

bool Variable_Data::decode( const Slice & result, uint8_t op )
{
    uint16_t nfields = 0;
    StreamBuf unpack( result.data(), result.size() );

    // fields number
    unpack.decode( nfields );

    // fields
    for ( uint16_t i = 0; i < nfields; ++i )
    {
        uint16_t index = 0;
        unpack.decode( index );

        switch ( index )
        {
            case 0 :
                m_dirty[0] = 1;
                unpack.decode( m_db_id );
                break;
            case 1 :
                m_dirty[1] = 1;
                unpack.decode( m_db_roleid );
                break;
            case 2 :
                m_dirty[2] = 1;
                unpack.decode( m_db_mailid );
                break;
            case 3 :
                m_dirty[3] = 1;
                unpack.decode( m_db_teamid );
                break;
            case 4 :
                m_dirty[4] = 1;
                unpack.decode( m_db_entityid );
                break;
            case 5 :
                m_dirty[5] = 1;
                unpack.decode( m_db_imageid );
                break;
            case 6 :
                m_dirty[6] = 1;
                unpack.decode( m_db_guildid );
                break;
            case 7 :
                m_dirty[7] = 1;
                unpack.decode( m_db_heroid );
                break;
            case 8 :
                m_dirty[8] = 1;
                unpack.decode( m_db_logid );
                break;
            case 9 :
                m_dirty[9] = 1;
                unpack.decode( m_db_messageid );
                break;
        }
    }

    // keys
    if ( op != eCodec_Query )
    {
        unpack.decode( m_db_id );
    }

    return true;
}

void Variable_Data::merge( const ISQLData * data )
{
    assert( data->tablename() == TABLENAME );
    const Variable_Data * d = static_cast<const Variable_Data *>(data);

    // fields
    for ( uint16_t i = 0; i < NFIELDS; ++i )
    {
        if ( d->m_dirty[i] == 0 )
        {
            continue;
        }

        switch ( i )
        {
            case 0 :
                m_dirty[0] = 1;
                set_db_id( d->get_db_id() );
                break;
            case 1 :
                m_dirty[1] = 1;
                set_db_roleid( d->get_db_roleid() );
                break;
            case 2 :
                m_dirty[2] = 1;
                set_db_mailid( d->get_db_mailid() );
                break;
            case 3 :
                m_dirty[3] = 1;
                set_db_teamid( d->get_db_teamid() );
                break;
            case 4 :
                m_dirty[4] = 1;
                set_db_entityid( d->get_db_entityid() );
                break;
            case 5 :
                m_dirty[5] = 1;
                set_db_imageid( d->get_db_imageid() );
                break;
            case 6 :
                m_dirty[6] = 1;
                set_db_guildid( d->get_db_guildid() );
                break;
            case 7 :
                m_dirty[7] = 1;
                set_db_heroid( d->get_db_heroid() );
                break;
            case 8 :
                m_dirty[8] = 1;
                set_db_logid( d->get_db_logid() );
                break;
            case 9 :
                m_dirty[9] = 1;
                set_db_messageid( d->get_db_messageid() );
                break;
        }
    }
}

bool Variable_Data::match( const ISQLData * data ) const
{
    assert( data->tablename() == TABLENAME );
    const Variable_Data * d = static_cast<const Variable_Data *>(data);

    // fields
    for ( uint16_t i = 0; i < NFIELDS; ++i )
    {
        if ( m_dirty[i] == 0 )
        {
            continue;
        }

        switch ( i )
        {
            case 0 :
                if ( get_db_id() != d->get_db_id() ) return false;
                break;
            case 1 :
                if ( get_db_roleid() != d->get_db_roleid() ) return false;
                break;
            case 2 :
                if ( get_db_mailid() != d->get_db_mailid() ) return false;
                break;
            case 3 :
                if ( get_db_teamid() != d->get_db_teamid() ) return false;
                break;
            case 4 :
                if ( get_db_entityid() != d->get_db_entityid() ) return false;
                break;
            case 5 :
                if ( get_db_imageid() != d->get_db_imageid() ) return false;
                break;
            case 6 :
                if ( get_db_guildid() != d->get_db_guildid() ) return false;
                break;
            case 7 :
                if ( get_db_heroid() != d->get_db_heroid() ) return false;
                break;
            case 8 :
                if ( get_db_logid() != d->get_db_logid() ) return false;
                break;
            case 9 :
                if ( get_db_messageid() != d->get_db_messageid() ) return false;
                break;
        }
    }
    return true;
}

bool Variable_Data::parse( const Slices & result )
{
    if ( result.size() != NFIELDS )
    {
        return false;
    }

    for ( uint16_t i = 0; i < NFIELDS; ++i )
    {
        switch ( i )
        {
            case 0 :
                m_db_id = (int16_t)std::atoi( result[i].ToString().c_str() );
                break;
            case 1 :
                m_db_roleid = (int64_t)std::atoll( result[i].ToString().c_str() );
                break;
            case 2 :
                m_db_mailid = (uint64_t)std::atoll( result[i].ToString().c_str() );
                break;
            case 3 :
                m_db_teamid = (uint64_t)std::atoll( result[i].ToString().c_str() );
                break;
            case 4 :
                m_db_entityid = (uint64_t)std::atoll( result[i].ToString().c_str() );
                break;
            case 5 :
                m_db_imageid = (uint64_t)std::atoll( result[i].ToString().c_str() );
                break;
            case 6 :
                m_db_guildid = (uint32_t)std::atoi( result[i].ToString().c_str() );
                break;
            case 7 :
                m_db_heroid = (uint64_t)std::atoll( result[i].ToString().c_str() );
                break;
            case 8 :
                m_db_logid = (uint64_t)std::atoll( result[i].ToString().c_str() );
                break;
            case 9 :
                m_db_messageid = (uint64_t)std::atoll( result[i].ToString().c_str() );
                break;
        }
    }

    return true;
}

void Variable_Data::registering( kaguya::State * state )
{
    (*state)["Variable_Data"].setClass( kaguya::UserdataMetatable<Variable_Data, ISQLData>()
        .setConstructors<Variable_Data(), Variable_Data(int16_t)>()
        .addFunction( "get_db_id", &Variable_Data::get_db_id )
        .addFunction( "set_db_id", &Variable_Data::set_db_id )
        .addFunction( "get_db_roleid", &Variable_Data::get_db_roleid )
        .addFunction( "set_db_roleid", &Variable_Data::set_db_roleid )
        .addFunction( "get_db_mailid", &Variable_Data::get_db_mailid )
        .addFunction( "set_db_mailid", &Variable_Data::set_db_mailid )
        .addFunction( "get_db_teamid", &Variable_Data::get_db_teamid )
        .addFunction( "set_db_teamid", &Variable_Data::set_db_teamid )
        .addFunction( "get_db_entityid", &Variable_Data::get_db_entityid )
        .addFunction( "set_db_entityid", &Variable_Data::set_db_entityid )
        .addFunction( "get_db_imageid", &Variable_Data::get_db_imageid )
        .addFunction( "set_db_imageid", &Variable_Data::set_db_imageid )
        .addFunction( "get_db_guildid", &Variable_Data::get_db_guildid )
        .addFunction( "set_db_guildid", &Variable_Data::set_db_guildid )
        .addFunction( "get_db_heroid", &Variable_Data::get_db_heroid )
        .addFunction( "set_db_heroid", &Variable_Data::set_db_heroid )
        .addFunction( "get_db_logid", &Variable_Data::get_db_logid )
        .addFunction( "set_db_logid", &Variable_Data::set_db_logid )
        .addFunction( "get_db_messageid", &Variable_Data::get_db_messageid )
        .addFunction( "set_db_messageid", &Variable_Data::set_db_messageid )
        .addStaticFunction( "convert", [] ( ISQLData * data ) -> Variable_Data * { return static_cast<Variable_Data *>( data ); } )
    );
}

bool Variable_Data::invoke( kaguya::State * state, const std::string & script )
{
    (*state)["Variable"] = this;
    state->dostring( script.c_str() );
    (*state)["Variable"] = nullptr;
    return true;
}

void Variable_Data::copyfrom( const Variable_Data & data )
{
    // Field: roleid, Index: 1
    m_dirty[1] = 1;
    m_db_roleid = data.get_db_roleid();
    // Field: mailid, Index: 2
    m_dirty[2] = 1;
    m_db_mailid = data.get_db_mailid();
    // Field: teamid, Index: 3
    m_dirty[3] = 1;
    m_db_teamid = data.get_db_teamid();
    // Field: entityid, Index: 4
    m_dirty[4] = 1;
    m_db_entityid = data.get_db_entityid();
    // Field: imageid, Index: 5
    m_dirty[5] = 1;
    m_db_imageid = data.get_db_imageid();
    // Field: guildid, Index: 6
    m_dirty[6] = 1;
    m_db_guildid = data.get_db_guildid();
    // Field: heroid, Index: 7
    m_dirty[7] = 1;
    m_db_heroid = data.get_db_heroid();
    // Field: logid, Index: 8
    m_dirty[8] = 1;
    m_db_logid = data.get_db_logid();
    // Field: messageid, Index: 9
    m_dirty[9] = 1;
    m_db_messageid = data.get_db_messageid();
}

}
