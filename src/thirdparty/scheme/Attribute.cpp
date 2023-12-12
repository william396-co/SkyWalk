
#include <cstdio>
#include <cassert>
#include <cinttypes>
#include <cstdlib>

#include "lua/kaguya.hpp"
#include "utils/integer.h"
#include "utils/streambuf.h"

#include "Attribute.hpp"

namespace data
{

// Tablename
const std::string Attribute_Data::TABLENAME = "Attribute";

// VARCHAR Length Limit
const size_t Attribute_Data::MAX_DB_SKILLS_LENGTH = 4096;
const size_t Attribute_Data::MAX_DB_ATTRIBUTE_LENGTH = 1024;

Attribute_Data::Attribute_Data()
{
    clean();
    // Fields
    m_db_id = 0;
    m_db_roleid = 0;
    m_db_formation = 0;
    m_db_order = 0;
    m_db_level = 0;
    m_db_star = 0;
    m_db_grade = 0;
    m_db_quality = 0;
    m_db_rate = 0;
    m_db_battlepoint = 0;
    m_db_baseid = 0;
    m_db_helper = 0;
    m_db_skills = "";
    m_db_attribute = "";
}

Attribute_Data::Attribute_Data( uint64_t id, int32_t formation, int32_t order, uint64_t roleid )
{
    clean();
    // Fields
    m_db_id = id;
    m_db_roleid = roleid;
    m_db_formation = formation;
    m_db_order = order;
    m_db_level = 0;
    m_db_star = 0;
    m_db_grade = 0;
    m_db_quality = 0;
    m_db_rate = 0;
    m_db_battlepoint = 0;
    m_db_baseid = 0;
    m_db_helper = 0;
    m_db_skills = "";
    m_db_attribute = "";
}

Attribute_Data::~Attribute_Data()
{
    m_db_id = 0;
    m_db_roleid = 0;
    m_db_formation = 0;
    m_db_order = 0;
    m_db_level = 0;
    m_db_star = 0;
    m_db_grade = 0;
    m_db_quality = 0;
    m_db_rate = 0;
    m_db_battlepoint = 0;
    m_db_baseid = 0;
    m_db_helper = 0;
    m_db_skills.clear();
    m_db_attribute.clear();
}

void Attribute_Data::clean()
{
    m_method = DBMethod::None;
    bzero( &m_dirty, sizeof(m_dirty) );
}

ISQLData * Attribute_Data::clone( bool isfull ) const
{
    Attribute_Data * data = new Attribute_Data();

    if ( data != nullptr && isfull )
    {
        data->m_method = m_method;
        memcpy( data->m_dirty, m_dirty, NFIELDS );

        data->m_db_id = m_db_id;
        data->m_db_roleid = m_db_roleid;
        data->m_db_formation = m_db_formation;
        data->m_db_order = m_db_order;
        data->m_db_level = m_db_level;
        data->m_db_star = m_db_star;
        data->m_db_grade = m_db_grade;
        data->m_db_quality = m_db_quality;
        data->m_db_rate = m_db_rate;
        data->m_db_battlepoint = m_db_battlepoint;
        data->m_db_baseid = m_db_baseid;
        data->m_db_helper = m_db_helper;
        data->m_db_skills = m_db_skills;
        data->m_db_attribute = m_db_attribute;
    }

    return data;
}

bool Attribute_Data::isDirty() const
{
    for ( uint16_t i = 0; i < NFIELDS; ++i )
    {
        if ( m_dirty[i] != 0 ) return true;
    }

    return false;
}

bool Attribute_Data::isKeydirty() const
{
    return m_dirty[0] == 1 || m_dirty[2] == 1 || m_dirty[3] == 1 || m_dirty[1] == 1;
}

void Attribute_Data::keystring( std::string & k ) const
{
    k = utils::Integer::toString( m_db_id );
    k += "#";
    k += utils::Integer::toString( m_db_formation );
    k += "#";
    k += utils::Integer::toString( m_db_order );
}

void Attribute_Data::indexstring( uint8_t op, std::string & k ) const
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

void Attribute_Data::autoincrease( uint8_t source, const Slice & value )
{
}

bool Attribute_Data::query( std::string & sqlcmd, std::vector<std::string> & escapevalues, bool all )
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
            if ( !fields.empty() && ( i == 0 || i == 2 || i == 3 || i == 1 ) )
            {
                fields += " AND ";
            }

            switch ( i )
            {
                case 0 :
                    fields += "`id`=";
                    fields += utils::Integer::toString( m_db_id );
                    break;
                case 2 :
                    fields += "`formation`=";
                    fields += utils::Integer::toString( m_db_formation );
                    break;
                case 3 :
                    fields += "`order`=";
                    fields += utils::Integer::toString( m_db_order );
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
        sqlcmd = "SELECT `id`,`roleid`,`formation`,`order`,`level`,`star`,`grade`,`quality`,`rate`,`battlepoint`,`baseid`,`helper`,`skills`,`attribute` FROM `Attribute_0`";
        sqlcmd += " union all ";
        sqlcmd += "SELECT `id`,`roleid`,`formation`,`order`,`level`,`star`,`grade`,`quality`,`rate`,`battlepoint`,`baseid`,`helper`,`skills`,`attribute` FROM `Attribute_1`";
        sqlcmd += " union all ";
        sqlcmd += "SELECT `id`,`roleid`,`formation`,`order`,`level`,`star`,`grade`,`quality`,`rate`,`battlepoint`,`baseid`,`helper`,`skills`,`attribute` FROM `Attribute_2`";
        sqlcmd += " union all ";
        sqlcmd += "SELECT `id`,`roleid`,`formation`,`order`,`level`,`star`,`grade`,`quality`,`rate`,`battlepoint`,`baseid`,`helper`,`skills`,`attribute` FROM `Attribute_3`";
    }
    else
    {
        uint32_t len = 32 + 126 + 0 + 9 + fields.size();
        sqlcmd.resize( len );
        uint32_t shard = m_db_roleid % 4;
        len = std::snprintf(
            (char *)sqlcmd.data(), len-1,
            "SELECT `id`,`roleid`,`formation`,`order`,`level`,`star`,`grade`,`quality`,`rate`,`battlepoint`,`baseid`,`helper`,`skills`,`attribute` FROM `Attribute_%u` WHERE %s", shard, fields.c_str() );
        sqlcmd.resize( len );
    }

    return true;
}

bool Attribute_Data::insert( std::string & sqlcmd, std::vector<std::string> & escapevalues )
{
    std::string fields, values;

    for ( uint16_t i = 0; i < NFIELDS; ++i )
    {
        if ( m_dirty[i] == 0 && i != 0 && i != 1 && i != 2 && i != 3 )
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
                fields += "`formation`";
                values += utils::Integer::toString( m_db_formation );
                break;
            case 3 :
                fields += "`order`";
                values += utils::Integer::toString( m_db_order );
                break;
            case 4 :
                fields += "`level`";
                values += utils::Integer::toString( m_db_level );
                break;
            case 5 :
                fields += "`star`";
                values += utils::Integer::toString( m_db_star );
                break;
            case 6 :
                fields += "`grade`";
                values += utils::Integer::toString( m_db_grade );
                break;
            case 7 :
                fields += "`quality`";
                values += utils::Integer::toString( m_db_quality );
                break;
            case 8 :
                fields += "`rate`";
                values += utils::Integer::toString( m_db_rate );
                break;
            case 9 :
                fields += "`battlepoint`";
                values += utils::Integer::toString( m_db_battlepoint );
                break;
            case 10 :
                fields += "`baseid`";
                values += utils::Integer::toString( m_db_baseid );
                break;
            case 11 :
                fields += "`helper`";
                values += utils::Integer::toString( m_db_helper );
                break;
            case 12 :
                fields += "`skills`";
                values += "'?'";
                escapevalues.push_back( m_db_skills );
                break;
            case 13 :
                fields += "`attribute`";
                values += "'?'";
                escapevalues.push_back( m_db_attribute );
                break;
        }
    }

    uint32_t len = 32 + 9 + fields.size() + values.size();

    sqlcmd.resize( len );
    uint32_t shard = m_db_roleid % 4;
    len = std::snprintf( (char *)sqlcmd.data(), len-1,
        "INSERT INTO `Attribute_%u` (%s) VALUES (%s)", shard, fields.c_str(), values.c_str() );
    sqlcmd.resize(len);

    return true;
}

bool Attribute_Data::update( std::string & sqlcmd, std::vector<std::string> & escapevalues )
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
                dirty += "`formation`=";
                dirty += utils::Integer::toString( m_db_formation );
                break;
            case 3 :
                dirty += "`order`=";
                dirty += utils::Integer::toString( m_db_order );
                break;
            case 4 :
                dirty += "`level`=";
                dirty += utils::Integer::toString( m_db_level );
                break;
            case 5 :
                dirty += "`star`=";
                dirty += utils::Integer::toString( m_db_star );
                break;
            case 6 :
                dirty += "`grade`=";
                dirty += utils::Integer::toString( m_db_grade );
                break;
            case 7 :
                dirty += "`quality`=";
                dirty += utils::Integer::toString( m_db_quality );
                break;
            case 8 :
                dirty += "`rate`=";
                dirty += utils::Integer::toString( m_db_rate );
                break;
            case 9 :
                dirty += "`battlepoint`=";
                dirty += utils::Integer::toString( m_db_battlepoint );
                break;
            case 10 :
                dirty += "`baseid`=";
                dirty += utils::Integer::toString( m_db_baseid );
                break;
            case 11 :
                dirty += "`helper`=";
                dirty += utils::Integer::toString( m_db_helper );
                break;
            case 12 :
                dirty += "`skills`=";
                dirty += "'?'";
                escapevalues.push_back( m_db_skills );
                break;
            case 13 :
                dirty += "`attribute`=";
                dirty += "'?'";
                escapevalues.push_back( m_db_attribute );
                break;
        }
    }

    int32_t len = 32 + 9 + dirty.size() + 50 + 32 + 32 + 32;

    sqlcmd.resize( len );
    uint32_t shard = m_db_roleid % 4;
    len = std::snprintf( (char *)sqlcmd.data(), len-1,
        "UPDATE `Attribute_%u` SET %s WHERE `id`=%" PRIu64 " AND `formation`=%d AND `order`=%d", shard, dirty.c_str(), m_db_id, m_db_formation, m_db_order );
    sqlcmd.resize( len );

    return true;
}

bool Attribute_Data::remove( std::string & sqlcmd, std::vector<std::string> & escapevalues )
{
    uint32_t len = 32 + 9 + 50 + 32 + 32 + 32;

    sqlcmd.resize( len );
    uint32_t shard = m_db_roleid % 4;
    len = std::snprintf( (char *)sqlcmd.data(), len-1,
        "DELETE FROM `Attribute_%u` WHERE `id`=%" PRIu64 " AND `formation`=%d AND `order`=%d", shard, m_db_id, m_db_formation, m_db_order );
    sqlcmd.resize( len );

    return true;
}

bool Attribute_Data::replace( std::string & sqlcmd, std::vector<std::string> & escapevalues )
{
    std::string fields, values, dirty;

    for ( uint16_t i = 0; i < NFIELDS; ++i )
    {
        if ( m_dirty[i] == 0 && i != 0 && i != 1 && i != 2 && i != 3 )
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
                fields += "`formation`";
                values += utils::Integer::toString( m_db_formation );
                break;
            case 3 :
                fields += "`order`";
                values += utils::Integer::toString( m_db_order );
                break;
            case 4 :
                fields += "`level`";
                values += utils::Integer::toString( m_db_level );
                break;
            case 5 :
                fields += "`star`";
                values += utils::Integer::toString( m_db_star );
                break;
            case 6 :
                fields += "`grade`";
                values += utils::Integer::toString( m_db_grade );
                break;
            case 7 :
                fields += "`quality`";
                values += utils::Integer::toString( m_db_quality );
                break;
            case 8 :
                fields += "`rate`";
                values += utils::Integer::toString( m_db_rate );
                break;
            case 9 :
                fields += "`battlepoint`";
                values += utils::Integer::toString( m_db_battlepoint );
                break;
            case 10 :
                fields += "`baseid`";
                values += utils::Integer::toString( m_db_baseid );
                break;
            case 11 :
                fields += "`helper`";
                values += utils::Integer::toString( m_db_helper );
                break;
            case 12 :
                fields += "`skills`";
                values += "'?'";
                escapevalues.push_back( m_db_skills );
                break;
            case 13 :
                fields += "`attribute`";
                values += "'?'";
                escapevalues.push_back( m_db_attribute );
                break;
        }
    }
    for ( uint16_t i = 0; i < NFIELDS; ++i )
    {
        if ( m_dirty[i] == 1 && i != 0 && i != 1 && i != 2 && i != 3 )
        {
            if ( !dirty.empty() )
            {
                dirty += ", ";
            }

            switch ( i )
            {
                case 4 :
                    dirty += "`level`=";
                    dirty += utils::Integer::toString( m_db_level );
                    break;
                case 5 :
                    dirty += "`star`=";
                    dirty += utils::Integer::toString( m_db_star );
                    break;
                case 6 :
                    dirty += "`grade`=";
                    dirty += utils::Integer::toString( m_db_grade );
                    break;
                case 7 :
                    dirty += "`quality`=";
                    dirty += utils::Integer::toString( m_db_quality );
                    break;
                case 8 :
                    dirty += "`rate`=";
                    dirty += utils::Integer::toString( m_db_rate );
                    break;
                case 9 :
                    dirty += "`battlepoint`=";
                    dirty += utils::Integer::toString( m_db_battlepoint );
                    break;
                case 10 :
                    dirty += "`baseid`=";
                    dirty += utils::Integer::toString( m_db_baseid );
                    break;
                case 11 :
                    dirty += "`helper`=";
                    dirty += utils::Integer::toString( m_db_helper );
                    break;
                case 12 :
                    dirty += "`skills`=";
                    dirty += "'?'";
                    escapevalues.push_back( m_db_skills );
                    break;
                case 13 :
                    dirty += "`attribute`=";
                    dirty += "'?'";
                    escapevalues.push_back( m_db_attribute );
                    break;
            }
        }
    }

    uint32_t len = 64 + 9 + fields.size() + values.size() + dirty.size();

    sqlcmd.resize( len );
    uint32_t shard = m_db_roleid % 4;
    len = std::snprintf( (char *)sqlcmd.data(), len-1,
        "INSERT INTO `Attribute_%u` (%s) VALUES (%s) ON DUPLICATE KEY UPDATE %s", shard, fields.c_str(), values.c_str(), dirty.c_str() );
    sqlcmd.resize(len);

    return true;
}

Slice Attribute_Data::encode( uint8_t op )
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
        if ( m_dirty[2] == 1 )
        {
            ++nfields;
            pack.encode( (uint16_t)2 );
            pack.encode( m_db_formation );
        }
        if ( m_dirty[3] == 1 )
        {
            ++nfields;
            pack.encode( (uint16_t)3 );
            pack.encode( m_db_order );
        }
        if ( m_dirty[1] == 1 )
        {
            ++nfields;
            pack.encode( (uint16_t)1 );
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
                    pack.encode( m_db_formation );
                    break;
                case 3 :
                    ++nfields;
                    pack.encode( i );
                    pack.encode( m_db_order );
                    break;
                case 4 :
                    ++nfields;
                    pack.encode( i );
                    pack.encode( m_db_level );
                    break;
                case 5 :
                    ++nfields;
                    pack.encode( i );
                    pack.encode( m_db_star );
                    break;
                case 6 :
                    ++nfields;
                    pack.encode( i );
                    pack.encode( m_db_grade );
                    break;
                case 7 :
                    ++nfields;
                    pack.encode( i );
                    pack.encode( m_db_quality );
                    break;
                case 8 :
                    ++nfields;
                    pack.encode( i );
                    pack.encode( m_db_rate );
                    break;
                case 9 :
                    ++nfields;
                    pack.encode( i );
                    pack.encode( m_db_battlepoint );
                    break;
                case 10 :
                    ++nfields;
                    pack.encode( i );
                    pack.encode( m_db_baseid );
                    break;
                case 11 :
                    ++nfields;
                    pack.encode( i );
                    pack.encode( m_db_helper );
                    break;
                case 12 :
                    ++nfields;
                    pack.encode( i );
                    pack.encode( m_db_skills );
                    break;
                case 13 :
                    ++nfields;
                    pack.encode( i );
                    pack.encode( m_db_attribute );
                    break;
            }
        }
    }

    // key fields
    if ( op != eCodec_Query )
    {
        pack.encode( m_db_id );
        pack.encode( m_db_formation );
        pack.encode( m_db_order );
        pack.encode( m_db_roleid );
    }

    // fields number
    pack.reset();
    pack.encode( nfields );

    return pack.slice();
}

bool Attribute_Data::decode( const Slice & result, uint8_t op )
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
                unpack.decode( m_db_formation );
                break;
            case 3 :
                m_dirty[3] = 1;
                unpack.decode( m_db_order );
                break;
            case 4 :
                m_dirty[4] = 1;
                unpack.decode( m_db_level );
                break;
            case 5 :
                m_dirty[5] = 1;
                unpack.decode( m_db_star );
                break;
            case 6 :
                m_dirty[6] = 1;
                unpack.decode( m_db_grade );
                break;
            case 7 :
                m_dirty[7] = 1;
                unpack.decode( m_db_quality );
                break;
            case 8 :
                m_dirty[8] = 1;
                unpack.decode( m_db_rate );
                break;
            case 9 :
                m_dirty[9] = 1;
                unpack.decode( m_db_battlepoint );
                break;
            case 10 :
                m_dirty[10] = 1;
                unpack.decode( m_db_baseid );
                break;
            case 11 :
                m_dirty[11] = 1;
                unpack.decode( m_db_helper );
                break;
            case 12 :
                m_dirty[12] = 1;
                unpack.decode( m_db_skills );
                break;
            case 13 :
                m_dirty[13] = 1;
                unpack.decode( m_db_attribute );
                break;
        }
    }

    // keys
    if ( op != eCodec_Query )
    {
        unpack.decode( m_db_id );
        unpack.decode( m_db_formation );
        unpack.decode( m_db_order );
        unpack.decode( m_db_roleid );
    }

    return true;
}

void Attribute_Data::merge( const ISQLData * data )
{
    assert( data->tablename() == TABLENAME );
    const Attribute_Data * d = static_cast<const Attribute_Data *>(data);

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
                set_db_formation( d->get_db_formation() );
                break;
            case 3 :
                m_dirty[3] = 1;
                set_db_order( d->get_db_order() );
                break;
            case 4 :
                m_dirty[4] = 1;
                set_db_level( d->get_db_level() );
                break;
            case 5 :
                m_dirty[5] = 1;
                set_db_star( d->get_db_star() );
                break;
            case 6 :
                m_dirty[6] = 1;
                set_db_grade( d->get_db_grade() );
                break;
            case 7 :
                m_dirty[7] = 1;
                set_db_quality( d->get_db_quality() );
                break;
            case 8 :
                m_dirty[8] = 1;
                set_db_rate( d->get_db_rate() );
                break;
            case 9 :
                m_dirty[9] = 1;
                set_db_battlepoint( d->get_db_battlepoint() );
                break;
            case 10 :
                m_dirty[10] = 1;
                set_db_baseid( d->get_db_baseid() );
                break;
            case 11 :
                m_dirty[11] = 1;
                set_db_helper( d->get_db_helper() );
                break;
            case 12 :
                m_dirty[12] = 1;
                set_db_skills( d->get_db_skills() );
                break;
            case 13 :
                m_dirty[13] = 1;
                set_db_attribute( d->get_db_attribute() );
                break;
        }
    }
}

bool Attribute_Data::match( const ISQLData * data ) const
{
    assert( data->tablename() == TABLENAME );
    const Attribute_Data * d = static_cast<const Attribute_Data *>(data);

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
                if ( get_db_formation() != d->get_db_formation() ) return false;
                break;
            case 3 :
                if ( get_db_order() != d->get_db_order() ) return false;
                break;
            case 4 :
                if ( get_db_level() != d->get_db_level() ) return false;
                break;
            case 5 :
                if ( get_db_star() != d->get_db_star() ) return false;
                break;
            case 6 :
                if ( get_db_grade() != d->get_db_grade() ) return false;
                break;
            case 7 :
                if ( get_db_quality() != d->get_db_quality() ) return false;
                break;
            case 8 :
                if ( get_db_rate() != d->get_db_rate() ) return false;
                break;
            case 9 :
                if ( get_db_battlepoint() != d->get_db_battlepoint() ) return false;
                break;
            case 10 :
                if ( get_db_baseid() != d->get_db_baseid() ) return false;
                break;
            case 11 :
                if ( get_db_helper() != d->get_db_helper() ) return false;
                break;
            case 12 :
                if ( get_db_skills() != d->get_db_skills() ) return false;
                break;
            case 13 :
                if ( get_db_attribute() != d->get_db_attribute() ) return false;
                break;
        }
    }
    return true;
}

bool Attribute_Data::parse( const Slices & result )
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
                m_db_formation = (int32_t)std::atoi( result[i].ToString().c_str() );
                break;
            case 3 :
                m_db_order = (int32_t)std::atoi( result[i].ToString().c_str() );
                break;
            case 4 :
                m_db_level = (int32_t)std::atoi( result[i].ToString().c_str() );
                break;
            case 5 :
                m_db_star = (int32_t)std::atoi( result[i].ToString().c_str() );
                break;
            case 6 :
                m_db_grade = (int32_t)std::atoi( result[i].ToString().c_str() );
                break;
            case 7 :
                m_db_quality = (int32_t)std::atoi( result[i].ToString().c_str() );
                break;
            case 8 :
                m_db_rate = (int32_t)std::atoi( result[i].ToString().c_str() );
                break;
            case 9 :
                m_db_battlepoint = (int64_t)std::atoll( result[i].ToString().c_str() );
                break;
            case 10 :
                m_db_baseid = (int32_t)std::atoi( result[i].ToString().c_str() );
                break;
            case 11 :
                m_db_helper = (int32_t)std::atoi( result[i].ToString().c_str() );
                break;
            case 12 :
                m_db_skills = result[i].ToString();
                break;
            case 13 :
                m_db_attribute = result[i].ToString();
                break;
        }
    }

    return true;
}

void Attribute_Data::registering( kaguya::State * state )
{
    (*state)["Attribute_Data"].setClass( kaguya::UserdataMetatable<Attribute_Data, ISQLData>()
        .setConstructors<Attribute_Data(), Attribute_Data(uint64_t, int32_t, int32_t, uint64_t)>()
        .addFunction( "get_db_id", &Attribute_Data::get_db_id )
        .addFunction( "set_db_id", &Attribute_Data::set_db_id )
        .addFunction( "get_db_roleid", &Attribute_Data::get_db_roleid )
        .addFunction( "set_db_roleid", &Attribute_Data::set_db_roleid )
        .addFunction( "get_db_formation", &Attribute_Data::get_db_formation )
        .addFunction( "set_db_formation", &Attribute_Data::set_db_formation )
        .addFunction( "get_db_order", &Attribute_Data::get_db_order )
        .addFunction( "set_db_order", &Attribute_Data::set_db_order )
        .addFunction( "get_db_level", &Attribute_Data::get_db_level )
        .addFunction( "set_db_level", &Attribute_Data::set_db_level )
        .addFunction( "get_db_star", &Attribute_Data::get_db_star )
        .addFunction( "set_db_star", &Attribute_Data::set_db_star )
        .addFunction( "get_db_grade", &Attribute_Data::get_db_grade )
        .addFunction( "set_db_grade", &Attribute_Data::set_db_grade )
        .addFunction( "get_db_quality", &Attribute_Data::get_db_quality )
        .addFunction( "set_db_quality", &Attribute_Data::set_db_quality )
        .addFunction( "get_db_rate", &Attribute_Data::get_db_rate )
        .addFunction( "set_db_rate", &Attribute_Data::set_db_rate )
        .addFunction( "get_db_battlepoint", &Attribute_Data::get_db_battlepoint )
        .addFunction( "set_db_battlepoint", &Attribute_Data::set_db_battlepoint )
        .addFunction( "get_db_baseid", &Attribute_Data::get_db_baseid )
        .addFunction( "set_db_baseid", &Attribute_Data::set_db_baseid )
        .addFunction( "get_db_helper", &Attribute_Data::get_db_helper )
        .addFunction( "set_db_helper", &Attribute_Data::set_db_helper )
        .addFunction( "get_db_skills", &Attribute_Data::get_db_skills )
        .addFunction( "set_db_skills", &Attribute_Data::set_db_skills )
        .addFunction( "get_db_attribute", &Attribute_Data::get_db_attribute )
        .addFunction( "set_db_attribute", &Attribute_Data::set_db_attribute )
        .addStaticFunction( "convert", [] ( ISQLData * data ) -> Attribute_Data * { return static_cast<Attribute_Data *>( data ); } )
    );
}

bool Attribute_Data::invoke( kaguya::State * state, const std::string & script )
{
    (*state)["Attribute"] = this;
    state->dostring( script.c_str() );
    (*state)["Attribute"] = nullptr;
    return true;
}

void Attribute_Data::copyfrom( const Attribute_Data & data )
{
    // Field: level, Index: 4
    m_dirty[4] = 1;
    m_db_level = data.get_db_level();
    // Field: star, Index: 5
    m_dirty[5] = 1;
    m_db_star = data.get_db_star();
    // Field: grade, Index: 6
    m_dirty[6] = 1;
    m_db_grade = data.get_db_grade();
    // Field: quality, Index: 7
    m_dirty[7] = 1;
    m_db_quality = data.get_db_quality();
    // Field: rate, Index: 8
    m_dirty[8] = 1;
    m_db_rate = data.get_db_rate();
    // Field: battlepoint, Index: 9
    m_dirty[9] = 1;
    m_db_battlepoint = data.get_db_battlepoint();
    // Field: baseid, Index: 10
    m_dirty[10] = 1;
    m_db_baseid = data.get_db_baseid();
    // Field: helper, Index: 11
    m_dirty[11] = 1;
    m_db_helper = data.get_db_helper();
    // Field: skills, Index: 12
    m_dirty[12] = 1;
    m_db_skills = data.get_db_skills();
    // Field: attribute, Index: 13
    m_dirty[13] = 1;
    m_db_attribute = data.get_db_attribute();
}

}
