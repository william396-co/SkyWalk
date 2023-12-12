
#include <cstdio>
#include <cassert>
#include <cinttypes>
#include <cstdlib>

#include "lua/kaguya.hpp"
#include "utils/integer.h"
#include "utils/streambuf.h"

#include "CombatJournal.hpp"

namespace data
{

// Tablename
const std::string CombatJournal_Data::TABLENAME = "CombatJournal";

// VARCHAR Length Limit
const size_t CombatJournal_Data::MAX_DB_NAME_LENGTH = 128;
const size_t CombatJournal_Data::MAX_DB_STATISTICS_LENGTH = 2048;
const size_t CombatJournal_Data::MAX_DB_EXTRAINFO_LENGTH = 1024;

CombatJournal_Data::CombatJournal_Data()
{
    clean();
    // Fields
    m_db_id = 0;
    m_db_roleid = 0;
    m_db_module = 0;
    m_db_result = 0;
    m_db_opponent = 0;
    m_db_robotid = 0;
    m_db_avatar = 0;
    m_db_name = "";
    m_db_level = 0;
    m_db_viplevel = 0;
    m_db_change = 0;
    m_db_timestamp = 0;
    m_db_statistics = "";
    m_db_extrainfo = "";
}

CombatJournal_Data::CombatJournal_Data( uint64_t id, uint64_t roleid )
{
    clean();
    // Fields
    m_db_id = id;
    m_db_roleid = roleid;
    m_db_module = 0;
    m_db_result = 0;
    m_db_opponent = 0;
    m_db_robotid = 0;
    m_db_avatar = 0;
    m_db_name = "";
    m_db_level = 0;
    m_db_viplevel = 0;
    m_db_change = 0;
    m_db_timestamp = 0;
    m_db_statistics = "";
    m_db_extrainfo = "";
}

CombatJournal_Data::~CombatJournal_Data()
{
    m_db_id = 0;
    m_db_roleid = 0;
    m_db_module = 0;
    m_db_result = 0;
    m_db_opponent = 0;
    m_db_robotid = 0;
    m_db_avatar = 0;
    m_db_name.clear();
    m_db_level = 0;
    m_db_viplevel = 0;
    m_db_change = 0;
    m_db_timestamp = 0;
    m_db_statistics.clear();
    m_db_extrainfo.clear();
}

void CombatJournal_Data::clean()
{
    m_method = DBMethod::None;
    bzero( &m_dirty, sizeof(m_dirty) );
}

ISQLData * CombatJournal_Data::clone( bool isfull ) const
{
    CombatJournal_Data * data = new CombatJournal_Data();

    if ( data != nullptr && isfull )
    {
        data->m_method = m_method;
        memcpy( data->m_dirty, m_dirty, NFIELDS );

        data->m_db_id = m_db_id;
        data->m_db_roleid = m_db_roleid;
        data->m_db_module = m_db_module;
        data->m_db_result = m_db_result;
        data->m_db_opponent = m_db_opponent;
        data->m_db_robotid = m_db_robotid;
        data->m_db_avatar = m_db_avatar;
        data->m_db_name = m_db_name;
        data->m_db_level = m_db_level;
        data->m_db_viplevel = m_db_viplevel;
        data->m_db_change = m_db_change;
        data->m_db_timestamp = m_db_timestamp;
        data->m_db_statistics = m_db_statistics;
        data->m_db_extrainfo = m_db_extrainfo;
    }

    return data;
}

bool CombatJournal_Data::isDirty() const
{
    for ( uint16_t i = 0; i < NFIELDS; ++i )
    {
        if ( m_dirty[i] != 0 ) return true;
    }

    return false;
}

bool CombatJournal_Data::isKeydirty() const
{
    return m_dirty[0] == 1 || m_dirty[1] == 1;
}

void CombatJournal_Data::keystring( std::string & k ) const
{
    k = utils::Integer::toString( m_db_id );
}

void CombatJournal_Data::indexstring( uint8_t op, std::string & k ) const
{
    if ( likely(op == eCodec_All) )
    {
        k = utils::Integer::toString( m_db_roleid );
    }
    else
    {
        if ( m_dirty[1] == 1 )
        {
            if ( !k.empty() ) k += "#";
            k += utils::Integer::toString( m_db_roleid );
        }
    }
}

void CombatJournal_Data::autoincrease( uint8_t source, const Slice & value )
{
}

bool CombatJournal_Data::query( std::string & sqlcmd, std::vector<std::string> & escapevalues, bool all )
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
            if ( !fields.empty() && ( i == 1 ) )
            {
                fields += " AND ";
            }

            switch ( i )
            {
                case 1 :
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
                    fields += "`id`=";
                    fields += utils::Integer::toString( m_db_id );
                    break;
                case 1 :
                    fields += "`roleid`=";
                    fields += utils::Integer::toString( m_db_roleid );
                    break;
            }
        }
    }

    if ( fields.empty() )
    {
        sqlcmd = "SELECT `id`,`roleid`,`module`,`result`,`opponent`,`robotid`,`avatar`,`name`,`level`,`viplevel`,`change`,`timestamp`,`statistics`,`extrainfo` FROM `CombatJournal`";
    }
    else
    {
        uint32_t len = 32 + 133 + 0 + 13 + fields.size();
        sqlcmd.resize( len );
        len = std::snprintf(
            (char *)sqlcmd.data(), len-1,
            "SELECT `id`,`roleid`,`module`,`result`,`opponent`,`robotid`,`avatar`,`name`,`level`,`viplevel`,`change`,`timestamp`,`statistics`,`extrainfo` FROM `CombatJournal` WHERE %s", fields.c_str() );
        sqlcmd.resize( len );
    }

    return true;
}

bool CombatJournal_Data::insert( std::string & sqlcmd, std::vector<std::string> & escapevalues )
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
                fields += "`id`";
                values += utils::Integer::toString( m_db_id );
                break;
            case 1 :
                fields += "`roleid`";
                values += utils::Integer::toString( m_db_roleid );
                break;
            case 2 :
                fields += "`module`";
                values += utils::Integer::toString( m_db_module );
                break;
            case 3 :
                fields += "`result`";
                values += utils::Integer::toString( m_db_result );
                break;
            case 4 :
                fields += "`opponent`";
                values += utils::Integer::toString( m_db_opponent );
                break;
            case 5 :
                fields += "`robotid`";
                values += utils::Integer::toString( m_db_robotid );
                break;
            case 6 :
                fields += "`avatar`";
                values += utils::Integer::toString( m_db_avatar );
                break;
            case 7 :
                fields += "`name`";
                values += "'?'";
                escapevalues.push_back( m_db_name );
                break;
            case 8 :
                fields += "`level`";
                values += utils::Integer::toString( m_db_level );
                break;
            case 9 :
                fields += "`viplevel`";
                values += utils::Integer::toString( m_db_viplevel );
                break;
            case 10 :
                fields += "`change`";
                values += utils::Integer::toString( m_db_change );
                break;
            case 11 :
                fields += "`timestamp`";
                values += utils::Integer::toString( m_db_timestamp );
                break;
            case 12 :
                fields += "`statistics`";
                values += "'?'";
                escapevalues.push_back( m_db_statistics );
                break;
            case 13 :
                fields += "`extrainfo`";
                values += "'?'";
                escapevalues.push_back( m_db_extrainfo );
                break;
        }
    }

    uint32_t len = 32 + 13 + fields.size() + values.size();

    sqlcmd.resize( len );
    len = std::snprintf( (char *)sqlcmd.data(), len-1,
        "INSERT INTO `CombatJournal` (%s) VALUES (%s)", fields.c_str(), values.c_str() );
    sqlcmd.resize(len);

    return true;
}

bool CombatJournal_Data::update( std::string & sqlcmd, std::vector<std::string> & escapevalues )
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
                dirty += "`module`=";
                dirty += utils::Integer::toString( m_db_module );
                break;
            case 3 :
                dirty += "`result`=";
                dirty += utils::Integer::toString( m_db_result );
                break;
            case 4 :
                dirty += "`opponent`=";
                dirty += utils::Integer::toString( m_db_opponent );
                break;
            case 5 :
                dirty += "`robotid`=";
                dirty += utils::Integer::toString( m_db_robotid );
                break;
            case 6 :
                dirty += "`avatar`=";
                dirty += utils::Integer::toString( m_db_avatar );
                break;
            case 7 :
                dirty += "`name`=";
                dirty += "'?'";
                escapevalues.push_back( m_db_name );
                break;
            case 8 :
                dirty += "`level`=";
                dirty += utils::Integer::toString( m_db_level );
                break;
            case 9 :
                dirty += "`viplevel`=";
                dirty += utils::Integer::toString( m_db_viplevel );
                break;
            case 10 :
                dirty += "`change`=";
                dirty += utils::Integer::toString( m_db_change );
                break;
            case 11 :
                dirty += "`timestamp`=";
                dirty += utils::Integer::toString( m_db_timestamp );
                break;
            case 12 :
                dirty += "`statistics`=";
                dirty += "'?'";
                escapevalues.push_back( m_db_statistics );
                break;
            case 13 :
                dirty += "`extrainfo`=";
                dirty += "'?'";
                escapevalues.push_back( m_db_extrainfo );
                break;
        }
    }

    int32_t len = 32 + 13 + dirty.size() + 16 + 32;

    sqlcmd.resize( len );
    len = std::snprintf( (char *)sqlcmd.data(), len-1,
        "UPDATE `CombatJournal` SET %s WHERE `id`=%" PRIu64 "", dirty.c_str(), m_db_id );
    sqlcmd.resize( len );

    return true;
}

bool CombatJournal_Data::remove( std::string & sqlcmd, std::vector<std::string> & escapevalues )
{
    uint32_t len = 32 + 13 + 16 + 32;

    sqlcmd.resize( len );
    len = std::snprintf( (char *)sqlcmd.data(), len-1,
        "DELETE FROM `CombatJournal` WHERE `id`=%" PRIu64 "", m_db_id );
    sqlcmd.resize( len );

    return true;
}

bool CombatJournal_Data::replace( std::string & sqlcmd, std::vector<std::string> & escapevalues )
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
                fields += "`id`";
                values += utils::Integer::toString( m_db_id );
                break;
            case 1 :
                fields += "`roleid`";
                values += utils::Integer::toString( m_db_roleid );
                break;
            case 2 :
                fields += "`module`";
                values += utils::Integer::toString( m_db_module );
                break;
            case 3 :
                fields += "`result`";
                values += utils::Integer::toString( m_db_result );
                break;
            case 4 :
                fields += "`opponent`";
                values += utils::Integer::toString( m_db_opponent );
                break;
            case 5 :
                fields += "`robotid`";
                values += utils::Integer::toString( m_db_robotid );
                break;
            case 6 :
                fields += "`avatar`";
                values += utils::Integer::toString( m_db_avatar );
                break;
            case 7 :
                fields += "`name`";
                values += "'?'";
                escapevalues.push_back( m_db_name );
                break;
            case 8 :
                fields += "`level`";
                values += utils::Integer::toString( m_db_level );
                break;
            case 9 :
                fields += "`viplevel`";
                values += utils::Integer::toString( m_db_viplevel );
                break;
            case 10 :
                fields += "`change`";
                values += utils::Integer::toString( m_db_change );
                break;
            case 11 :
                fields += "`timestamp`";
                values += utils::Integer::toString( m_db_timestamp );
                break;
            case 12 :
                fields += "`statistics`";
                values += "'?'";
                escapevalues.push_back( m_db_statistics );
                break;
            case 13 :
                fields += "`extrainfo`";
                values += "'?'";
                escapevalues.push_back( m_db_extrainfo );
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
                    dirty += "`module`=";
                    dirty += utils::Integer::toString( m_db_module );
                    break;
                case 3 :
                    dirty += "`result`=";
                    dirty += utils::Integer::toString( m_db_result );
                    break;
                case 4 :
                    dirty += "`opponent`=";
                    dirty += utils::Integer::toString( m_db_opponent );
                    break;
                case 5 :
                    dirty += "`robotid`=";
                    dirty += utils::Integer::toString( m_db_robotid );
                    break;
                case 6 :
                    dirty += "`avatar`=";
                    dirty += utils::Integer::toString( m_db_avatar );
                    break;
                case 7 :
                    dirty += "`name`=";
                    dirty += "'?'";
                    escapevalues.push_back( m_db_name );
                    break;
                case 8 :
                    dirty += "`level`=";
                    dirty += utils::Integer::toString( m_db_level );
                    break;
                case 9 :
                    dirty += "`viplevel`=";
                    dirty += utils::Integer::toString( m_db_viplevel );
                    break;
                case 10 :
                    dirty += "`change`=";
                    dirty += utils::Integer::toString( m_db_change );
                    break;
                case 11 :
                    dirty += "`timestamp`=";
                    dirty += utils::Integer::toString( m_db_timestamp );
                    break;
                case 12 :
                    dirty += "`statistics`=";
                    dirty += "'?'";
                    escapevalues.push_back( m_db_statistics );
                    break;
                case 13 :
                    dirty += "`extrainfo`=";
                    dirty += "'?'";
                    escapevalues.push_back( m_db_extrainfo );
                    break;
            }
        }
    }

    uint32_t len = 64 + 13 + fields.size() + values.size() + dirty.size();

    sqlcmd.resize( len );
    len = std::snprintf( (char *)sqlcmd.data(), len-1,
        "INSERT INTO `CombatJournal` (%s) VALUES (%s) ON DUPLICATE KEY UPDATE %s", fields.c_str(), values.c_str(), dirty.c_str() );
    sqlcmd.resize(len);

    return true;
}

Slice CombatJournal_Data::encode( uint8_t op )
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
        if ( m_dirty[1] == 1 )
        {
            ++nfields;
            pack.encode( (uint16_t)1 );
            pack.encode( m_db_roleid );
        }
        if ( m_dirty[2] == 1 )
        {
            ++nfields;
            pack.encode( (uint16_t)2 );
            pack.encode( m_db_module );
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
                    pack.encode( m_db_module );
                    break;
                case 3 :
                    ++nfields;
                    pack.encode( i );
                    pack.encode( m_db_result );
                    break;
                case 4 :
                    ++nfields;
                    pack.encode( i );
                    pack.encode( m_db_opponent );
                    break;
                case 5 :
                    ++nfields;
                    pack.encode( i );
                    pack.encode( m_db_robotid );
                    break;
                case 6 :
                    ++nfields;
                    pack.encode( i );
                    pack.encode( m_db_avatar );
                    break;
                case 7 :
                    ++nfields;
                    pack.encode( i );
                    pack.encode( m_db_name );
                    break;
                case 8 :
                    ++nfields;
                    pack.encode( i );
                    pack.encode( m_db_level );
                    break;
                case 9 :
                    ++nfields;
                    pack.encode( i );
                    pack.encode( m_db_viplevel );
                    break;
                case 10 :
                    ++nfields;
                    pack.encode( i );
                    pack.encode( m_db_change );
                    break;
                case 11 :
                    ++nfields;
                    pack.encode( i );
                    pack.encode( m_db_timestamp );
                    break;
                case 12 :
                    ++nfields;
                    pack.encode( i );
                    pack.encode( m_db_statistics );
                    break;
                case 13 :
                    ++nfields;
                    pack.encode( i );
                    pack.encode( m_db_extrainfo );
                    break;
            }
        }
    }

    // key fields
    if ( op != eCodec_Query )
    {
        pack.encode( m_db_id );
        pack.encode( m_db_roleid );
    }

    // fields number
    pack.reset();
    pack.encode( nfields );

    return pack.slice();
}

bool CombatJournal_Data::decode( const Slice & result, uint8_t op )
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
                unpack.decode( m_db_module );
                break;
            case 3 :
                m_dirty[3] = 1;
                unpack.decode( m_db_result );
                break;
            case 4 :
                m_dirty[4] = 1;
                unpack.decode( m_db_opponent );
                break;
            case 5 :
                m_dirty[5] = 1;
                unpack.decode( m_db_robotid );
                break;
            case 6 :
                m_dirty[6] = 1;
                unpack.decode( m_db_avatar );
                break;
            case 7 :
                m_dirty[7] = 1;
                unpack.decode( m_db_name );
                break;
            case 8 :
                m_dirty[8] = 1;
                unpack.decode( m_db_level );
                break;
            case 9 :
                m_dirty[9] = 1;
                unpack.decode( m_db_viplevel );
                break;
            case 10 :
                m_dirty[10] = 1;
                unpack.decode( m_db_change );
                break;
            case 11 :
                m_dirty[11] = 1;
                unpack.decode( m_db_timestamp );
                break;
            case 12 :
                m_dirty[12] = 1;
                unpack.decode( m_db_statistics );
                break;
            case 13 :
                m_dirty[13] = 1;
                unpack.decode( m_db_extrainfo );
                break;
        }
    }

    // keys
    if ( op != eCodec_Query )
    {
        unpack.decode( m_db_id );
        unpack.decode( m_db_roleid );
    }

    return true;
}

void CombatJournal_Data::merge( const ISQLData * data )
{
    assert( data->tablename() == TABLENAME );
    const CombatJournal_Data * d = static_cast<const CombatJournal_Data *>(data);

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
                set_db_module( d->get_db_module() );
                break;
            case 3 :
                m_dirty[3] = 1;
                set_db_result( d->get_db_result() );
                break;
            case 4 :
                m_dirty[4] = 1;
                set_db_opponent( d->get_db_opponent() );
                break;
            case 5 :
                m_dirty[5] = 1;
                set_db_robotid( d->get_db_robotid() );
                break;
            case 6 :
                m_dirty[6] = 1;
                set_db_avatar( d->get_db_avatar() );
                break;
            case 7 :
                m_dirty[7] = 1;
                set_db_name( d->get_db_name() );
                break;
            case 8 :
                m_dirty[8] = 1;
                set_db_level( d->get_db_level() );
                break;
            case 9 :
                m_dirty[9] = 1;
                set_db_viplevel( d->get_db_viplevel() );
                break;
            case 10 :
                m_dirty[10] = 1;
                set_db_change( d->get_db_change() );
                break;
            case 11 :
                m_dirty[11] = 1;
                set_db_timestamp( d->get_db_timestamp() );
                break;
            case 12 :
                m_dirty[12] = 1;
                set_db_statistics( d->get_db_statistics() );
                break;
            case 13 :
                m_dirty[13] = 1;
                set_db_extrainfo( d->get_db_extrainfo() );
                break;
        }
    }
}

bool CombatJournal_Data::match( const ISQLData * data ) const
{
    assert( data->tablename() == TABLENAME );
    const CombatJournal_Data * d = static_cast<const CombatJournal_Data *>(data);

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
                if ( get_db_module() != d->get_db_module() ) return false;
                break;
            case 3 :
                if ( get_db_result() != d->get_db_result() ) return false;
                break;
            case 4 :
                if ( get_db_opponent() != d->get_db_opponent() ) return false;
                break;
            case 5 :
                if ( get_db_robotid() != d->get_db_robotid() ) return false;
                break;
            case 6 :
                if ( get_db_avatar() != d->get_db_avatar() ) return false;
                break;
            case 7 :
                if ( get_db_name() != d->get_db_name() ) return false;
                break;
            case 8 :
                if ( get_db_level() != d->get_db_level() ) return false;
                break;
            case 9 :
                if ( get_db_viplevel() != d->get_db_viplevel() ) return false;
                break;
            case 10 :
                if ( get_db_change() != d->get_db_change() ) return false;
                break;
            case 11 :
                if ( get_db_timestamp() != d->get_db_timestamp() ) return false;
                break;
            case 12 :
                if ( get_db_statistics() != d->get_db_statistics() ) return false;
                break;
            case 13 :
                if ( get_db_extrainfo() != d->get_db_extrainfo() ) return false;
                break;
        }
    }
    return true;
}

bool CombatJournal_Data::parse( const Slices & result )
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
                m_db_id = (uint64_t)std::atoll( result[i].ToString().c_str() );
                break;
            case 1 :
                m_db_roleid = (uint64_t)std::atoll( result[i].ToString().c_str() );
                break;
            case 2 :
                m_db_module = (int32_t)std::atoi( result[i].ToString().c_str() );
                break;
            case 3 :
                m_db_result = (int32_t)std::atoi( result[i].ToString().c_str() );
                break;
            case 4 :
                m_db_opponent = (uint64_t)std::atoll( result[i].ToString().c_str() );
                break;
            case 5 :
                m_db_robotid = (int32_t)std::atoi( result[i].ToString().c_str() );
                break;
            case 6 :
                m_db_avatar = (uint32_t)std::atoi( result[i].ToString().c_str() );
                break;
            case 7 :
                m_db_name = result[i].ToString();
                break;
            case 8 :
                m_db_level = (int32_t)std::atoi( result[i].ToString().c_str() );
                break;
            case 9 :
                m_db_viplevel = (int32_t)std::atoi( result[i].ToString().c_str() );
                break;
            case 10 :
                m_db_change = (int32_t)std::atoi( result[i].ToString().c_str() );
                break;
            case 11 :
                m_db_timestamp = (int64_t)std::atoll( result[i].ToString().c_str() );
                break;
            case 12 :
                m_db_statistics = result[i].ToString();
                break;
            case 13 :
                m_db_extrainfo = result[i].ToString();
                break;
        }
    }

    return true;
}

void CombatJournal_Data::registering( kaguya::State * state )
{
    (*state)["CombatJournal_Data"].setClass( kaguya::UserdataMetatable<CombatJournal_Data, ISQLData>()
        .setConstructors<CombatJournal_Data(), CombatJournal_Data(uint64_t, uint64_t)>()
        .addFunction( "get_db_id", &CombatJournal_Data::get_db_id )
        .addFunction( "set_db_id", &CombatJournal_Data::set_db_id )
        .addFunction( "get_db_roleid", &CombatJournal_Data::get_db_roleid )
        .addFunction( "set_db_roleid", &CombatJournal_Data::set_db_roleid )
        .addFunction( "get_db_module", &CombatJournal_Data::get_db_module )
        .addFunction( "set_db_module", &CombatJournal_Data::set_db_module )
        .addFunction( "get_db_result", &CombatJournal_Data::get_db_result )
        .addFunction( "set_db_result", &CombatJournal_Data::set_db_result )
        .addFunction( "get_db_opponent", &CombatJournal_Data::get_db_opponent )
        .addFunction( "set_db_opponent", &CombatJournal_Data::set_db_opponent )
        .addFunction( "get_db_robotid", &CombatJournal_Data::get_db_robotid )
        .addFunction( "set_db_robotid", &CombatJournal_Data::set_db_robotid )
        .addFunction( "get_db_avatar", &CombatJournal_Data::get_db_avatar )
        .addFunction( "set_db_avatar", &CombatJournal_Data::set_db_avatar )
        .addFunction( "get_db_name", &CombatJournal_Data::get_db_name )
        .addFunction( "set_db_name", &CombatJournal_Data::set_db_name )
        .addFunction( "get_db_level", &CombatJournal_Data::get_db_level )
        .addFunction( "set_db_level", &CombatJournal_Data::set_db_level )
        .addFunction( "get_db_viplevel", &CombatJournal_Data::get_db_viplevel )
        .addFunction( "set_db_viplevel", &CombatJournal_Data::set_db_viplevel )
        .addFunction( "get_db_change", &CombatJournal_Data::get_db_change )
        .addFunction( "set_db_change", &CombatJournal_Data::set_db_change )
        .addFunction( "get_db_timestamp", &CombatJournal_Data::get_db_timestamp )
        .addFunction( "set_db_timestamp", &CombatJournal_Data::set_db_timestamp )
        .addFunction( "get_db_statistics", &CombatJournal_Data::get_db_statistics )
        .addFunction( "set_db_statistics", &CombatJournal_Data::set_db_statistics )
        .addFunction( "get_db_extrainfo", &CombatJournal_Data::get_db_extrainfo )
        .addFunction( "set_db_extrainfo", &CombatJournal_Data::set_db_extrainfo )
        .addStaticFunction( "convert", [] ( ISQLData * data ) -> CombatJournal_Data * { return static_cast<CombatJournal_Data *>( data ); } )
    );
}

bool CombatJournal_Data::invoke( kaguya::State * state, const std::string & script )
{
    (*state)["CombatJournal"] = this;
    state->dostring( script.c_str() );
    (*state)["CombatJournal"] = nullptr;
    return true;
}

void CombatJournal_Data::copyfrom( const CombatJournal_Data & data )
{
    // Field: module, Index: 2
    m_dirty[2] = 1;
    m_db_module = data.get_db_module();
    // Field: result, Index: 3
    m_dirty[3] = 1;
    m_db_result = data.get_db_result();
    // Field: opponent, Index: 4
    m_dirty[4] = 1;
    m_db_opponent = data.get_db_opponent();
    // Field: robotid, Index: 5
    m_dirty[5] = 1;
    m_db_robotid = data.get_db_robotid();
    // Field: avatar, Index: 6
    m_dirty[6] = 1;
    m_db_avatar = data.get_db_avatar();
    // Field: name, Index: 7
    m_dirty[7] = 1;
    m_db_name = data.get_db_name();
    // Field: level, Index: 8
    m_dirty[8] = 1;
    m_db_level = data.get_db_level();
    // Field: viplevel, Index: 9
    m_dirty[9] = 1;
    m_db_viplevel = data.get_db_viplevel();
    // Field: change, Index: 10
    m_dirty[10] = 1;
    m_db_change = data.get_db_change();
    // Field: timestamp, Index: 11
    m_dirty[11] = 1;
    m_db_timestamp = data.get_db_timestamp();
    // Field: statistics, Index: 12
    m_dirty[12] = 1;
    m_db_statistics = data.get_db_statistics();
    // Field: extrainfo, Index: 13
    m_dirty[13] = 1;
    m_db_extrainfo = data.get_db_extrainfo();
}

}
