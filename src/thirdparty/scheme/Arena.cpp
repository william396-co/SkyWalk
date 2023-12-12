
#include <cstdio>
#include <cassert>
#include <cinttypes>
#include <cstdlib>

#include "lua/kaguya.hpp"
#include "utils/streambuf.h"

#include "Arena.hpp"

namespace data
{

// Tablename
const std::string Arena_Data::TABLENAME = "Arena";

// VARCHAR Length Limit
const size_t Arena_Data::MAX_DB_OPPONENTS_LENGTH = 256;

Arena_Data::Arena_Data()
{
    clean();
    // Fields
    m_db_roleid = 0;
    m_db_timestamp = 0;
    m_db_opponents = "";
    m_db_lastrank = 0;
    m_db_gettimestamp = 0;
    m_db_drawtimestamp = 0;
    m_db_drawhltimestamp = 0;
}

Arena_Data::Arena_Data( uint64_t roleid )
{
    clean();
    // Fields
    m_db_roleid = roleid;
    m_db_timestamp = 0;
    m_db_opponents = "";
    m_db_lastrank = 0;
    m_db_gettimestamp = 0;
    m_db_drawtimestamp = 0;
    m_db_drawhltimestamp = 0;
}

Arena_Data::~Arena_Data()
{
    m_db_roleid = 0;
    m_db_timestamp = 0;
    m_db_opponents.clear();
    m_db_lastrank = 0;
    m_db_gettimestamp = 0;
    m_db_drawtimestamp = 0;
    m_db_drawhltimestamp = 0;
}

void Arena_Data::clean()
{
    m_method = DBMethod::None;
    bzero( &m_dirty, sizeof(m_dirty) );
}

ISQLData * Arena_Data::clone( bool isfull ) const
{
    Arena_Data * data = new Arena_Data();

    if ( data != nullptr && isfull )
    {
        data->m_method = m_method;
        memcpy( data->m_dirty, m_dirty, NFIELDS );

        data->m_db_roleid = m_db_roleid;
        data->m_db_timestamp = m_db_timestamp;
        data->m_db_opponents = m_db_opponents;
        data->m_db_lastrank = m_db_lastrank;
        data->m_db_gettimestamp = m_db_gettimestamp;
        data->m_db_drawtimestamp = m_db_drawtimestamp;
        data->m_db_drawhltimestamp = m_db_drawhltimestamp;
    }

    return data;
}

bool Arena_Data::isDirty() const
{
    for ( uint16_t i = 0; i < NFIELDS; ++i )
    {
        if ( m_dirty[i] != 0 ) return true;
    }

    return false;
}

bool Arena_Data::isKeydirty() const
{
    return m_dirty[0] == 1;
}

void Arena_Data::keystring( std::string & k ) const
{
    k = std::to_string( m_db_roleid );
}

void Arena_Data::indexstring( uint8_t op, std::string & k ) const
{
    if ( likely(op == eCodec_All) )
    {
        k = std::to_string( m_db_roleid );
    }
    else
    {
        if ( m_dirty[0] == 1 )
        {
            if ( !k.empty() ) k += "#";
            k += std::to_string( m_db_roleid );
        }
    }
}

void Arena_Data::autoincrease( uint8_t source, const Slice & value )
{
}

bool Arena_Data::query( std::string & sqlcmd, std::vector<std::string> & escapevalues, bool all )
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
                fields += "`roleid`=";
                fields += std::to_string( m_db_roleid );
                break;
        }
    }

    if ( fields.empty() )
    {
        sqlcmd = "SELECT `roleid`,`timestamp`,`opponents`,`lastrank`,`gettimestamp`,`drawtimestamp`,`drawhltimestamp` FROM `Arena`";
    }
    else
    {
        uint32_t len = 32 + 92 + 8 + 5 + fields.size();
        sqlcmd.resize( len );
        len = std::snprintf(
            (char *)sqlcmd.data(), len-1,
            "SELECT `roleid`,`timestamp`,`opponents`,`lastrank`,`gettimestamp`,`drawtimestamp`,`drawhltimestamp` FROM `Arena` WHERE %s LIMIT 1", fields.c_str() );
        sqlcmd.resize( len );
    }

    return true;
}

bool Arena_Data::insert( std::string & sqlcmd, std::vector<std::string> & escapevalues )
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
                fields += "`roleid`";
                values += std::to_string( m_db_roleid );
                break;
            case 1 :
                fields += "`timestamp`";
                values += std::to_string( m_db_timestamp );
                break;
            case 2 :
                fields += "`opponents`";
                values += "'?'";
                escapevalues.push_back( m_db_opponents );
                break;
            case 3 :
                fields += "`lastrank`";
                values += std::to_string( m_db_lastrank );
                break;
            case 4 :
                fields += "`gettimestamp`";
                values += std::to_string( m_db_gettimestamp );
                break;
            case 5 :
                fields += "`drawtimestamp`";
                values += std::to_string( m_db_drawtimestamp );
                break;
            case 6 :
                fields += "`drawhltimestamp`";
                values += std::to_string( m_db_drawhltimestamp );
                break;
        }
    }

    uint32_t len = 32 + 5 + fields.size() + values.size();

    sqlcmd.resize( len );
    len = std::snprintf( (char *)sqlcmd.data(), len-1,
        "INSERT INTO `Arena` (%s) VALUES (%s)", fields.c_str(), values.c_str() );
    sqlcmd.resize(len);

    return true;
}

bool Arena_Data::update( std::string & sqlcmd, std::vector<std::string> & escapevalues )
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
                dirty += "`roleid`=";
                dirty += std::to_string( m_db_roleid );
                break;
            case 1 :
                dirty += "`timestamp`=";
                dirty += std::to_string( m_db_timestamp );
                break;
            case 2 :
                dirty += "`opponents`=";
                dirty += "'?'";
                escapevalues.push_back( m_db_opponents );
                break;
            case 3 :
                dirty += "`lastrank`=";
                dirty += std::to_string( m_db_lastrank );
                break;
            case 4 :
                dirty += "`gettimestamp`=";
                dirty += std::to_string( m_db_gettimestamp );
                break;
            case 5 :
                dirty += "`drawtimestamp`=";
                dirty += std::to_string( m_db_drawtimestamp );
                break;
            case 6 :
                dirty += "`drawhltimestamp`=";
                dirty += std::to_string( m_db_drawhltimestamp );
                break;
        }
    }

    int32_t len = 32 + 5 + dirty.size() + 20 + 32;

    sqlcmd.resize( len );
    len = std::snprintf( (char *)sqlcmd.data(), len-1,
        "UPDATE `Arena` SET %s WHERE `roleid`=%" PRIu64 "", dirty.c_str(), m_db_roleid );
    sqlcmd.resize( len );

    return true;
}

bool Arena_Data::remove( std::string & sqlcmd, std::vector<std::string> & escapevalues )
{
    uint32_t len = 32 + 5 + 20 + 32;

    sqlcmd.resize( len );
    len = std::snprintf( (char *)sqlcmd.data(), len-1,
        "DELETE FROM `Arena` WHERE `roleid`=%" PRIu64 "", m_db_roleid );
    sqlcmd.resize( len );

    return true;
}

bool Arena_Data::replace( std::string & sqlcmd, std::vector<std::string> & escapevalues )
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
                fields += "`roleid`";
                values += std::to_string( m_db_roleid );
                break;
            case 1 :
                fields += "`timestamp`";
                values += std::to_string( m_db_timestamp );
                break;
            case 2 :
                fields += "`opponents`";
                values += "'?'";
                escapevalues.push_back( m_db_opponents );
                break;
            case 3 :
                fields += "`lastrank`";
                values += std::to_string( m_db_lastrank );
                break;
            case 4 :
                fields += "`gettimestamp`";
                values += std::to_string( m_db_gettimestamp );
                break;
            case 5 :
                fields += "`drawtimestamp`";
                values += std::to_string( m_db_drawtimestamp );
                break;
            case 6 :
                fields += "`drawhltimestamp`";
                values += std::to_string( m_db_drawhltimestamp );
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
                    dirty += "`timestamp`=";
                    dirty += std::to_string( m_db_timestamp );
                    break;
                case 2 :
                    dirty += "`opponents`=";
                    dirty += "'?'";
                    escapevalues.push_back( m_db_opponents );
                    break;
                case 3 :
                    dirty += "`lastrank`=";
                    dirty += std::to_string( m_db_lastrank );
                    break;
                case 4 :
                    dirty += "`gettimestamp`=";
                    dirty += std::to_string( m_db_gettimestamp );
                    break;
                case 5 :
                    dirty += "`drawtimestamp`=";
                    dirty += std::to_string( m_db_drawtimestamp );
                    break;
                case 6 :
                    dirty += "`drawhltimestamp`=";
                    dirty += std::to_string( m_db_drawhltimestamp );
                    break;
            }
        }
    }

    uint32_t len = 64 + 5 + fields.size() + values.size() + dirty.size();

    sqlcmd.resize( len );
    len = std::snprintf( (char *)sqlcmd.data(), len-1,
        "INSERT INTO `Arena` (%s) VALUES (%s) ON DUPLICATE KEY UPDATE %s", fields.c_str(), values.c_str(), dirty.c_str() );
    sqlcmd.resize(len);

    return true;
}

Slice Arena_Data::encode( uint8_t op )
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
            pack.encode( m_db_roleid );
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
                    pack.encode( m_db_roleid );
                    break;
                case 1 :
                    ++nfields;
                    pack.encode( i );
                    pack.encode( m_db_timestamp );
                    break;
                case 2 :
                    ++nfields;
                    pack.encode( i );
                    pack.encode( m_db_opponents );
                    break;
                case 3 :
                    ++nfields;
                    pack.encode( i );
                    pack.encode( m_db_lastrank );
                    break;
                case 4 :
                    ++nfields;
                    pack.encode( i );
                    pack.encode( m_db_gettimestamp );
                    break;
                case 5 :
                    ++nfields;
                    pack.encode( i );
                    pack.encode( m_db_drawtimestamp );
                    break;
                case 6 :
                    ++nfields;
                    pack.encode( i );
                    pack.encode( m_db_drawhltimestamp );
                    break;
            }
        }
    }

    // key fields
    if ( op != eCodec_Query )
    {
        pack.encode( m_db_roleid );
    }

    // fields number
    pack.reset();
    pack.encode( nfields );

    return pack.slice();
}

bool Arena_Data::decode( const Slice & result, uint8_t op )
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
                unpack.decode( m_db_roleid );
                break;
            case 1 :
                m_dirty[1] = 1;
                unpack.decode( m_db_timestamp );
                break;
            case 2 :
                m_dirty[2] = 1;
                unpack.decode( m_db_opponents );
                break;
            case 3 :
                m_dirty[3] = 1;
                unpack.decode( m_db_lastrank );
                break;
            case 4 :
                m_dirty[4] = 1;
                unpack.decode( m_db_gettimestamp );
                break;
            case 5 :
                m_dirty[5] = 1;
                unpack.decode( m_db_drawtimestamp );
                break;
            case 6 :
                m_dirty[6] = 1;
                unpack.decode( m_db_drawhltimestamp );
                break;
        }
    }

    // keys
    if ( op != eCodec_Query )
    {
        unpack.decode( m_db_roleid );
    }

    return true;
}

void Arena_Data::merge( const ISQLData * data )
{
    assert( data->tablename() == TABLENAME );
    const Arena_Data * d = static_cast<const Arena_Data *>(data);

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
                set_db_roleid( d->get_db_roleid() );
                break;
            case 1 :
                m_dirty[1] = 1;
                set_db_timestamp( d->get_db_timestamp() );
                break;
            case 2 :
                m_dirty[2] = 1;
                set_db_opponents( d->get_db_opponents() );
                break;
            case 3 :
                m_dirty[3] = 1;
                set_db_lastrank( d->get_db_lastrank() );
                break;
            case 4 :
                m_dirty[4] = 1;
                set_db_gettimestamp( d->get_db_gettimestamp() );
                break;
            case 5 :
                m_dirty[5] = 1;
                set_db_drawtimestamp( d->get_db_drawtimestamp() );
                break;
            case 6 :
                m_dirty[6] = 1;
                set_db_drawhltimestamp( d->get_db_drawhltimestamp() );
                break;
        }
    }
}

bool Arena_Data::match( const ISQLData * data ) const
{
    assert( data->tablename() == TABLENAME );
    const Arena_Data * d = static_cast<const Arena_Data *>(data);

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
                if ( get_db_roleid() != d->get_db_roleid() ) return false;
                break;
            case 1 :
                if ( get_db_timestamp() != d->get_db_timestamp() ) return false;
                break;
            case 2 :
                if ( get_db_opponents() != d->get_db_opponents() ) return false;
                break;
            case 3 :
                if ( get_db_lastrank() != d->get_db_lastrank() ) return false;
                break;
            case 4 :
                if ( get_db_gettimestamp() != d->get_db_gettimestamp() ) return false;
                break;
            case 5 :
                if ( get_db_drawtimestamp() != d->get_db_drawtimestamp() ) return false;
                break;
            case 6 :
                if ( get_db_drawhltimestamp() != d->get_db_drawhltimestamp() ) return false;
                break;
        }
    }
    return true;
}

bool Arena_Data::parse( const Slices & result )
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
                m_db_roleid = (uint64_t)std::atoll( result[i].ToString().c_str() );
                break;
            case 1 :
                m_db_timestamp = (int64_t)std::atoll( result[i].ToString().c_str() );
                break;
            case 2 :
                m_db_opponents = result[i].ToString();
                break;
            case 3 :
                m_db_lastrank = (int32_t)std::atoi( result[i].ToString().c_str() );
                break;
            case 4 :
                m_db_gettimestamp = (int64_t)std::atoll( result[i].ToString().c_str() );
                break;
            case 5 :
                m_db_drawtimestamp = (int64_t)std::atoll( result[i].ToString().c_str() );
                break;
            case 6 :
                m_db_drawhltimestamp = (int64_t)std::atoll( result[i].ToString().c_str() );
                break;
        }
    }

    return true;
}

void Arena_Data::registering( kaguya::State * state )
{
    (*state)["Arena_Data"].setClass( kaguya::UserdataMetatable<Arena_Data, ISQLData>()
        .setConstructors<Arena_Data(), Arena_Data(uint64_t)>()
        .addFunction( "get_db_roleid", &Arena_Data::get_db_roleid )
        .addFunction( "set_db_roleid", &Arena_Data::set_db_roleid )
        .addFunction( "get_db_timestamp", &Arena_Data::get_db_timestamp )
        .addFunction( "set_db_timestamp", &Arena_Data::set_db_timestamp )
        .addFunction( "get_db_opponents", &Arena_Data::get_db_opponents )
        .addFunction( "set_db_opponents", &Arena_Data::set_db_opponents )
        .addFunction( "get_db_lastrank", &Arena_Data::get_db_lastrank )
        .addFunction( "set_db_lastrank", &Arena_Data::set_db_lastrank )
        .addFunction( "get_db_gettimestamp", &Arena_Data::get_db_gettimestamp )
        .addFunction( "set_db_gettimestamp", &Arena_Data::set_db_gettimestamp )
        .addFunction( "get_db_drawtimestamp", &Arena_Data::get_db_drawtimestamp )
        .addFunction( "set_db_drawtimestamp", &Arena_Data::set_db_drawtimestamp )
        .addFunction( "get_db_drawhltimestamp", &Arena_Data::get_db_drawhltimestamp )
        .addFunction( "set_db_drawhltimestamp", &Arena_Data::set_db_drawhltimestamp )
        .addStaticFunction( "convert", [] ( ISQLData * data ) -> Arena_Data * { return static_cast<Arena_Data *>( data ); } )
    );
}

bool Arena_Data::invoke( kaguya::State * state, const std::string & script )
{
    (*state)["Arena"] = this;
    state->dostring( script.c_str() );
    (*state)["Arena"] = nullptr;
    return true;
}

void Arena_Data::copyfrom( const Arena_Data & data )
{
    // Field: timestamp, Index: 1
    m_dirty[1] = 1;
    m_db_timestamp = data.get_db_timestamp();
    // Field: opponents, Index: 2
    m_dirty[2] = 1;
    m_db_opponents = data.get_db_opponents();
    // Field: lastrank, Index: 3
    m_dirty[3] = 1;
    m_db_lastrank = data.get_db_lastrank();
    // Field: gettimestamp, Index: 4
    m_dirty[4] = 1;
    m_db_gettimestamp = data.get_db_gettimestamp();
    // Field: drawtimestamp, Index: 5
    m_dirty[5] = 1;
    m_db_drawtimestamp = data.get_db_drawtimestamp();
    // Field: drawhltimestamp, Index: 6
    m_dirty[6] = 1;
    m_db_drawhltimestamp = data.get_db_drawhltimestamp();
}

}
