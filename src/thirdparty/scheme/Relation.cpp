
#include <cstdio>
#include <cassert>
#include <cinttypes>
#include <cstdlib>

#include "lua/kaguya.hpp"
#include "utils/integer.h"
#include "utils/streambuf.h"

#include "Relation.hpp"

namespace data
{

// Tablename
const std::string Relation_Data::TABLENAME = "Relation";

// VARCHAR Length Limit

Relation_Data::Relation_Data()
{
    clean();
    // Fields
    m_db_roleid = 0;
    m_db_friendid = 0;
    m_db_type = 0;
    m_db_intimacy = 0;
    m_db_timestamp = 0;
}

Relation_Data::Relation_Data( uint64_t roleid, uint64_t friendid )
{
    clean();
    // Fields
    m_db_roleid = roleid;
    m_db_friendid = friendid;
    m_db_type = 0;
    m_db_intimacy = 0;
    m_db_timestamp = 0;
}

Relation_Data::~Relation_Data()
{
    m_db_roleid = 0;
    m_db_friendid = 0;
    m_db_type = 0;
    m_db_intimacy = 0;
    m_db_timestamp = 0;
}

void Relation_Data::clean()
{
    m_method = DBMethod::None;
    bzero( &m_dirty, sizeof(m_dirty) );
}

ISQLData * Relation_Data::clone( bool isfull ) const
{
    Relation_Data * data = new Relation_Data();

    if ( data != nullptr && isfull )
    {
        data->m_method = m_method;
        memcpy( data->m_dirty, m_dirty, NFIELDS );

        data->m_db_roleid = m_db_roleid;
        data->m_db_friendid = m_db_friendid;
        data->m_db_type = m_db_type;
        data->m_db_intimacy = m_db_intimacy;
        data->m_db_timestamp = m_db_timestamp;
    }

    return data;
}

bool Relation_Data::isDirty() const
{
    for ( uint16_t i = 0; i < NFIELDS; ++i )
    {
        if ( m_dirty[i] != 0 ) return true;
    }

    return false;
}

bool Relation_Data::isKeydirty() const
{
    return m_dirty[0] == 1 || m_dirty[1] == 1;
}

void Relation_Data::keystring( std::string & k ) const
{
    k = utils::Integer::toString( m_db_roleid );
    k += "#";
    k += utils::Integer::toString( m_db_friendid );
}

void Relation_Data::indexstring( uint8_t op, std::string & k ) const
{
    if ( likely(op == eCodec_All) )
    {
        k = utils::Integer::toString( m_db_roleid );
    }
    else
    {
        if ( m_dirty[0] == 1 )
        {
            if ( !k.empty() ) k += "#";
            k += utils::Integer::toString( m_db_roleid );
        }
    }
}

void Relation_Data::autoincrease( uint8_t source, const Slice & value )
{
}

bool Relation_Data::query( std::string & sqlcmd, std::vector<std::string> & escapevalues, bool all )
{
    std::string fields;

    for ( uint16_t i = 0; i < NFIELDS; ++i )
    {
        if ( m_dirty[i] == 0 )
        {
            continue;
        }

        if ( all )
        {
            if ( !fields.empty() && ( i == 0 ) )
            {
                fields += " AND ";
            }

            switch ( i )
            {
                case 0 :
                    fields += "`roleid`=";
                    fields += utils::Integer::toString( m_db_roleid );
                    break;
            }
        }
        else
        {
            if ( !fields.empty() && ( i == 0 || i == 1 ) )
            {
                fields += " AND ";
            }

            switch ( i )
            {
                case 0 :
                    fields += "`roleid`=";
                    fields += utils::Integer::toString( m_db_roleid );
                    break;
                case 1 :
                    fields += "`friendid`=";
                    fields += utils::Integer::toString( m_db_friendid );
                    break;
            }
        }
    }

    if ( fields.empty() )
    {
        sqlcmd = "SELECT `roleid`,`friendid`,`type`,`intimacy`,`timestamp` FROM `Relation`";
    }
    else
    {
        uint32_t len = 32 + 49 + 0 + 8 + fields.size();
        sqlcmd.resize( len );
        len = std::snprintf(
            (char *)sqlcmd.data(), len-1,
            "SELECT `roleid`,`friendid`,`type`,`intimacy`,`timestamp` FROM `Relation` WHERE %s", fields.c_str() );
        sqlcmd.resize( len );
    }

    return true;
}

bool Relation_Data::insert( std::string & sqlcmd, std::vector<std::string> & escapevalues )
{
    std::string fields, values;

    for ( uint16_t i = 0; i < NFIELDS; ++i )
    {
        if ( m_dirty[i] == 0 && i != 0 && i != 1 )
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
                values += utils::Integer::toString( m_db_roleid );
                break;
            case 1 :
                fields += "`friendid`";
                values += utils::Integer::toString( m_db_friendid );
                break;
            case 2 :
                fields += "`type`";
                values += utils::Integer::toString( m_db_type );
                break;
            case 3 :
                fields += "`intimacy`";
                values += utils::Integer::toString( m_db_intimacy );
                break;
            case 4 :
                fields += "`timestamp`";
                values += utils::Integer::toString( m_db_timestamp );
                break;
        }
    }

    uint32_t len = 32 + 8 + fields.size() + values.size();

    sqlcmd.resize( len );
    len = std::snprintf( (char *)sqlcmd.data(), len-1,
        "INSERT INTO `Relation` (%s) VALUES (%s)", fields.c_str(), values.c_str() );
    sqlcmd.resize(len);

    return true;
}

bool Relation_Data::update( std::string & sqlcmd, std::vector<std::string> & escapevalues )
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
                dirty += utils::Integer::toString( m_db_roleid );
                break;
            case 1 :
                dirty += "`friendid`=";
                dirty += utils::Integer::toString( m_db_friendid );
                break;
            case 2 :
                dirty += "`type`=";
                dirty += utils::Integer::toString( m_db_type );
                break;
            case 3 :
                dirty += "`intimacy`=";
                dirty += utils::Integer::toString( m_db_intimacy );
                break;
            case 4 :
                dirty += "`timestamp`=";
                dirty += utils::Integer::toString( m_db_timestamp );
                break;
        }
    }

    int32_t len = 32 + 8 + dirty.size() + 47 + 32 + 32;

    sqlcmd.resize( len );
    len = std::snprintf( (char *)sqlcmd.data(), len-1,
        "UPDATE `Relation` SET %s WHERE `roleid`=%" PRIu64 " AND `friendid`=%" PRIu64 "", dirty.c_str(), m_db_roleid, m_db_friendid );
    sqlcmd.resize( len );

    return true;
}

bool Relation_Data::remove( std::string & sqlcmd, std::vector<std::string> & escapevalues )
{
    uint32_t len = 32 + 8 + 47 + 32 + 32;

    sqlcmd.resize( len );
    len = std::snprintf( (char *)sqlcmd.data(), len-1,
        "DELETE FROM `Relation` WHERE `roleid`=%" PRIu64 " AND `friendid`=%" PRIu64 "", m_db_roleid, m_db_friendid );
    sqlcmd.resize( len );

    return true;
}

bool Relation_Data::replace( std::string & sqlcmd, std::vector<std::string> & escapevalues )
{
    std::string fields, values, dirty;

    for ( uint16_t i = 0; i < NFIELDS; ++i )
    {
        if ( m_dirty[i] == 0 && i != 0 && i != 1 )
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
                values += utils::Integer::toString( m_db_roleid );
                break;
            case 1 :
                fields += "`friendid`";
                values += utils::Integer::toString( m_db_friendid );
                break;
            case 2 :
                fields += "`type`";
                values += utils::Integer::toString( m_db_type );
                break;
            case 3 :
                fields += "`intimacy`";
                values += utils::Integer::toString( m_db_intimacy );
                break;
            case 4 :
                fields += "`timestamp`";
                values += utils::Integer::toString( m_db_timestamp );
                break;
        }
    }
    for ( uint16_t i = 0; i < NFIELDS; ++i )
    {
        if ( m_dirty[i] == 1 && i != 0 && i != 1 )
        {
            if ( !dirty.empty() )
            {
                dirty += ", ";
            }

            switch ( i )
            {
                case 2 :
                    dirty += "`type`=";
                    dirty += utils::Integer::toString( m_db_type );
                    break;
                case 3 :
                    dirty += "`intimacy`=";
                    dirty += utils::Integer::toString( m_db_intimacy );
                    break;
                case 4 :
                    dirty += "`timestamp`=";
                    dirty += utils::Integer::toString( m_db_timestamp );
                    break;
            }
        }
    }

    uint32_t len = 64 + 8 + fields.size() + values.size() + dirty.size();

    sqlcmd.resize( len );
    len = std::snprintf( (char *)sqlcmd.data(), len-1,
        "INSERT INTO `Relation` (%s) VALUES (%s) ON DUPLICATE KEY UPDATE %s", fields.c_str(), values.c_str(), dirty.c_str() );
    sqlcmd.resize(len);

    return true;
}

Slice Relation_Data::encode( uint8_t op )
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
        if ( m_dirty[1] == 1 )
        {
            ++nfields;
            pack.encode( (uint16_t)1 );
            pack.encode( m_db_friendid );
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
                    pack.encode( m_db_friendid );
                    break;
                case 2 :
                    ++nfields;
                    pack.encode( i );
                    pack.encode( m_db_type );
                    break;
                case 3 :
                    ++nfields;
                    pack.encode( i );
                    pack.encode( m_db_intimacy );
                    break;
                case 4 :
                    ++nfields;
                    pack.encode( i );
                    pack.encode( m_db_timestamp );
                    break;
            }
        }
    }

    // key fields
    if ( op != eCodec_Query )
    {
        pack.encode( m_db_roleid );
        pack.encode( m_db_friendid );
    }

    // fields number
    pack.reset();
    pack.encode( nfields );

    return pack.slice();
}

bool Relation_Data::decode( const Slice & result, uint8_t op )
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
                unpack.decode( m_db_friendid );
                break;
            case 2 :
                m_dirty[2] = 1;
                unpack.decode( m_db_type );
                break;
            case 3 :
                m_dirty[3] = 1;
                unpack.decode( m_db_intimacy );
                break;
            case 4 :
                m_dirty[4] = 1;
                unpack.decode( m_db_timestamp );
                break;
        }
    }

    // keys
    if ( op != eCodec_Query )
    {
        unpack.decode( m_db_roleid );
        unpack.decode( m_db_friendid );
    }

    return true;
}

void Relation_Data::merge( const ISQLData * data )
{
    assert( data->tablename() == TABLENAME );
    const Relation_Data * d = static_cast<const Relation_Data *>(data);

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
                set_db_friendid( d->get_db_friendid() );
                break;
            case 2 :
                m_dirty[2] = 1;
                set_db_type( d->get_db_type() );
                break;
            case 3 :
                m_dirty[3] = 1;
                set_db_intimacy( d->get_db_intimacy() );
                break;
            case 4 :
                m_dirty[4] = 1;
                set_db_timestamp( d->get_db_timestamp() );
                break;
        }
    }
}

bool Relation_Data::match( const ISQLData * data ) const
{
    assert( data->tablename() == TABLENAME );
    const Relation_Data * d = static_cast<const Relation_Data *>(data);

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
                if ( get_db_friendid() != d->get_db_friendid() ) return false;
                break;
            case 2 :
                if ( get_db_type() != d->get_db_type() ) return false;
                break;
            case 3 :
                if ( get_db_intimacy() != d->get_db_intimacy() ) return false;
                break;
            case 4 :
                if ( get_db_timestamp() != d->get_db_timestamp() ) return false;
                break;
        }
    }
    return true;
}

bool Relation_Data::parse( const Slices & result )
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
                m_db_friendid = (uint64_t)std::atoll( result[i].ToString().c_str() );
                break;
            case 2 :
                m_db_type = (uint8_t)std::atoi( result[i].ToString().c_str() );
                break;
            case 3 :
                m_db_intimacy = (uint32_t)std::atoi( result[i].ToString().c_str() );
                break;
            case 4 :
                m_db_timestamp = (uint64_t)std::atoll( result[i].ToString().c_str() );
                break;
        }
    }

    return true;
}

void Relation_Data::registering( kaguya::State * state )
{
    (*state)["Relation_Data"].setClass( kaguya::UserdataMetatable<Relation_Data, ISQLData>()
        .setConstructors<Relation_Data(), Relation_Data(uint64_t, uint64_t)>()
        .addFunction( "get_db_roleid", &Relation_Data::get_db_roleid )
        .addFunction( "set_db_roleid", &Relation_Data::set_db_roleid )
        .addFunction( "get_db_friendid", &Relation_Data::get_db_friendid )
        .addFunction( "set_db_friendid", &Relation_Data::set_db_friendid )
        .addFunction( "get_db_type", &Relation_Data::get_db_type )
        .addFunction( "set_db_type", &Relation_Data::set_db_type )
        .addFunction( "get_db_intimacy", &Relation_Data::get_db_intimacy )
        .addFunction( "set_db_intimacy", &Relation_Data::set_db_intimacy )
        .addFunction( "get_db_timestamp", &Relation_Data::get_db_timestamp )
        .addFunction( "set_db_timestamp", &Relation_Data::set_db_timestamp )
        .addStaticFunction( "convert", [] ( ISQLData * data ) -> Relation_Data * { return static_cast<Relation_Data *>( data ); } )
    );
}

bool Relation_Data::invoke( kaguya::State * state, const std::string & script )
{
    (*state)["Relation"] = this;
    state->dostring( script.c_str() );
    (*state)["Relation"] = nullptr;
    return true;
}

void Relation_Data::copyfrom( const Relation_Data & data )
{
    // Field: type, Index: 2
    m_dirty[2] = 1;
    m_db_type = data.get_db_type();
    // Field: intimacy, Index: 3
    m_dirty[3] = 1;
    m_db_intimacy = data.get_db_intimacy();
    // Field: timestamp, Index: 4
    m_dirty[4] = 1;
    m_db_timestamp = data.get_db_timestamp();
}

}
