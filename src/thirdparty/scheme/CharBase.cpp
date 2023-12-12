
#include <cstdio>
#include <cassert>
#include <cinttypes>
#include <cstdlib>

#include "lua/kaguya.hpp"
#include "utils/integer.h"
#include "utils/streambuf.h"

#include "CharBase.hpp"

namespace data
{

// Tablename
const std::string CharBase_Data::TABLENAME = "CharBase";

// VARCHAR Length Limit
const size_t CharBase_Data::MAX_DB_NAME_LENGTH = 255;
const size_t CharBase_Data::MAX_DB_ACCOUNT_LENGTH = 255;
const size_t CharBase_Data::MAX_DB_LASTPOSITION_LENGTH = 50;
const size_t CharBase_Data::MAX_DB_OPENSYSTEMS_LENGTH = 1024;

CharBase_Data::CharBase_Data()
{
    clean();
    // Fields
    m_db_roleid = 0;
    m_db_zone = 0;
    m_db_createtime = 0;
    m_db_lastlogintime = 0;
    m_db_lastlogouttime = 0;
    m_db_exp = 0;
    m_db_money = 0;
    m_db_diamond = 0;
    m_db_gamecoin = 0;
    m_db_arenacoin = 0;
    m_db_strength = 0;
    m_db_strengthtimestamp = 0;
    m_db_vipexp = 0;
    m_db_lastcity = 0;
    m_db_lastposition = "";
    m_db_totembagcapacity = 0;
    m_db_guildcoin = 0;
    m_db_opensystems = "";
}

CharBase_Data::CharBase_Data( uint64_t roleid )
{
    clean();
    // Fields
    m_db_roleid = roleid;
    m_db_zone = 0;
    m_db_createtime = 0;
    m_db_lastlogintime = 0;
    m_db_lastlogouttime = 0;
    m_db_exp = 0;
    m_db_money = 0;
    m_db_diamond = 0;
    m_db_gamecoin = 0;
    m_db_arenacoin = 0;
    m_db_strength = 0;
    m_db_strengthtimestamp = 0;
    m_db_vipexp = 0;
    m_db_lastcity = 0;
    m_db_lastposition = "";
    m_db_totembagcapacity = 0;
    m_db_guildcoin = 0;
    m_db_opensystems = "";
}

CharBase_Data::~CharBase_Data()
{
    m_db_roleid = 0;
    m_db_name.clear();
    m_db_account.clear();
    m_db_zone = 0;
    m_db_createtime = 0;
    m_db_lastlogintime = 0;
    m_db_lastlogouttime = 0;
    m_db_exp = 0;
    m_db_money = 0;
    m_db_diamond = 0;
    m_db_gamecoin = 0;
    m_db_arenacoin = 0;
    m_db_strength = 0;
    m_db_strengthtimestamp = 0;
    m_db_vipexp = 0;
    m_db_lastcity = 0;
    m_db_lastposition.clear();
    m_db_totembagcapacity = 0;
    m_db_guildcoin = 0;
    m_db_opensystems.clear();
}

void CharBase_Data::clean()
{
    m_method = DBMethod::None;
    bzero( &m_dirty, sizeof(m_dirty) );
}

ISQLData * CharBase_Data::clone( bool isfull ) const
{
    CharBase_Data * data = new CharBase_Data();

    if ( data != nullptr && isfull )
    {
        data->m_method = m_method;
        memcpy( data->m_dirty, m_dirty, NFIELDS );

        data->m_db_roleid = m_db_roleid;
        data->m_db_name = m_db_name;
        data->m_db_account = m_db_account;
        data->m_db_zone = m_db_zone;
        data->m_db_createtime = m_db_createtime;
        data->m_db_lastlogintime = m_db_lastlogintime;
        data->m_db_lastlogouttime = m_db_lastlogouttime;
        data->m_db_exp = m_db_exp;
        data->m_db_money = m_db_money;
        data->m_db_diamond = m_db_diamond;
        data->m_db_gamecoin = m_db_gamecoin;
        data->m_db_arenacoin = m_db_arenacoin;
        data->m_db_strength = m_db_strength;
        data->m_db_strengthtimestamp = m_db_strengthtimestamp;
        data->m_db_vipexp = m_db_vipexp;
        data->m_db_lastcity = m_db_lastcity;
        data->m_db_lastposition = m_db_lastposition;
        data->m_db_totembagcapacity = m_db_totembagcapacity;
        data->m_db_guildcoin = m_db_guildcoin;
        data->m_db_opensystems = m_db_opensystems;
    }

    return data;
}

bool CharBase_Data::isDirty() const
{
    for ( uint16_t i = 0; i < NFIELDS; ++i )
    {
        if ( m_dirty[i] != 0 ) return true;
    }

    return false;
}

bool CharBase_Data::isKeydirty() const
{
    return m_dirty[0] == 1;
}

void CharBase_Data::keystring( std::string & k ) const
{
    k = utils::Integer::toString( m_db_roleid );
}

void CharBase_Data::indexstring( uint8_t op, std::string & k ) const
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

void CharBase_Data::autoincrease( uint8_t source, const Slice & value )
{
}

bool CharBase_Data::query( std::string & sqlcmd, std::vector<std::string> & escapevalues, bool all )
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
                fields += utils::Integer::toString( m_db_roleid );
                break;
        }
    }

    if ( fields.empty() )
    {
        sqlcmd = "SELECT `roleid`,`name`,`account`,`zone`,`createtime`,`lastlogintime`,`lastlogouttime`,`exp`,`money`,`diamond`,`gamecoin`,`arenacoin`,`strength`,`strengthtimestamp`,`vipexp`,`lastcity`,`lastposition`,`totembagcapacity`,`guildcoin`,`opensystems` FROM `CharBase`";
    }
    else
    {
        uint32_t len = 32 + 236 + 8 + 8 + fields.size();
        sqlcmd.resize( len );
        len = std::snprintf(
            (char *)sqlcmd.data(), len-1,
            "SELECT `roleid`,`name`,`account`,`zone`,`createtime`,`lastlogintime`,`lastlogouttime`,`exp`,`money`,`diamond`,`gamecoin`,`arenacoin`,`strength`,`strengthtimestamp`,`vipexp`,`lastcity`,`lastposition`,`totembagcapacity`,`guildcoin`,`opensystems` FROM `CharBase` WHERE %s LIMIT 1", fields.c_str() );
        sqlcmd.resize( len );
    }

    return true;
}

bool CharBase_Data::insert( std::string & sqlcmd, std::vector<std::string> & escapevalues )
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
                values += utils::Integer::toString( m_db_roleid );
                break;
            case 1 :
                fields += "`name`";
                values += "'?'";
                escapevalues.push_back( m_db_name );
                break;
            case 2 :
                fields += "`account`";
                values += "'?'";
                escapevalues.push_back( m_db_account );
                break;
            case 3 :
                fields += "`zone`";
                values += utils::Integer::toString( m_db_zone );
                break;
            case 4 :
                fields += "`createtime`";
                values += utils::Integer::toString( m_db_createtime );
                break;
            case 5 :
                fields += "`lastlogintime`";
                values += utils::Integer::toString( m_db_lastlogintime );
                break;
            case 6 :
                fields += "`lastlogouttime`";
                values += utils::Integer::toString( m_db_lastlogouttime );
                break;
            case 7 :
                fields += "`exp`";
                values += utils::Integer::toString( m_db_exp );
                break;
            case 8 :
                fields += "`money`";
                values += utils::Integer::toString( m_db_money );
                break;
            case 9 :
                fields += "`diamond`";
                values += utils::Integer::toString( m_db_diamond );
                break;
            case 10 :
                fields += "`gamecoin`";
                values += utils::Integer::toString( m_db_gamecoin );
                break;
            case 11 :
                fields += "`arenacoin`";
                values += utils::Integer::toString( m_db_arenacoin );
                break;
            case 12 :
                fields += "`strength`";
                values += utils::Integer::toString( m_db_strength );
                break;
            case 13 :
                fields += "`strengthtimestamp`";
                values += utils::Integer::toString( m_db_strengthtimestamp );
                break;
            case 14 :
                fields += "`vipexp`";
                values += utils::Integer::toString( m_db_vipexp );
                break;
            case 15 :
                fields += "`lastcity`";
                values += utils::Integer::toString( m_db_lastcity );
                break;
            case 16 :
                fields += "`lastposition`";
                values += "'?'";
                escapevalues.push_back( m_db_lastposition );
                break;
            case 17 :
                fields += "`totembagcapacity`";
                values += utils::Integer::toString( m_db_totembagcapacity );
                break;
            case 18 :
                fields += "`guildcoin`";
                values += utils::Integer::toString( m_db_guildcoin );
                break;
            case 19 :
                fields += "`opensystems`";
                values += "'?'";
                escapevalues.push_back( m_db_opensystems );
                break;
        }
    }

    uint32_t len = 32 + 8 + fields.size() + values.size();

    sqlcmd.resize( len );
    len = std::snprintf( (char *)sqlcmd.data(), len-1,
        "INSERT INTO `CharBase` (%s) VALUES (%s)", fields.c_str(), values.c_str() );
    sqlcmd.resize(len);

    return true;
}

bool CharBase_Data::update( std::string & sqlcmd, std::vector<std::string> & escapevalues )
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
                dirty += "`name`=";
                dirty += "'?'";
                escapevalues.push_back( m_db_name );
                break;
            case 2 :
                dirty += "`account`=";
                dirty += "'?'";
                escapevalues.push_back( m_db_account );
                break;
            case 3 :
                dirty += "`zone`=";
                dirty += utils::Integer::toString( m_db_zone );
                break;
            case 4 :
                dirty += "`createtime`=";
                dirty += utils::Integer::toString( m_db_createtime );
                break;
            case 5 :
                dirty += "`lastlogintime`=";
                dirty += utils::Integer::toString( m_db_lastlogintime );
                break;
            case 6 :
                dirty += "`lastlogouttime`=";
                dirty += utils::Integer::toString( m_db_lastlogouttime );
                break;
            case 7 :
                dirty += "`exp`=";
                dirty += utils::Integer::toString( m_db_exp );
                break;
            case 8 :
                dirty += "`money`=";
                dirty += utils::Integer::toString( m_db_money );
                break;
            case 9 :
                dirty += "`diamond`=";
                dirty += utils::Integer::toString( m_db_diamond );
                break;
            case 10 :
                dirty += "`gamecoin`=";
                dirty += utils::Integer::toString( m_db_gamecoin );
                break;
            case 11 :
                dirty += "`arenacoin`=";
                dirty += utils::Integer::toString( m_db_arenacoin );
                break;
            case 12 :
                dirty += "`strength`=";
                dirty += utils::Integer::toString( m_db_strength );
                break;
            case 13 :
                dirty += "`strengthtimestamp`=";
                dirty += utils::Integer::toString( m_db_strengthtimestamp );
                break;
            case 14 :
                dirty += "`vipexp`=";
                dirty += utils::Integer::toString( m_db_vipexp );
                break;
            case 15 :
                dirty += "`lastcity`=";
                dirty += utils::Integer::toString( m_db_lastcity );
                break;
            case 16 :
                dirty += "`lastposition`=";
                dirty += "'?'";
                escapevalues.push_back( m_db_lastposition );
                break;
            case 17 :
                dirty += "`totembagcapacity`=";
                dirty += utils::Integer::toString( m_db_totembagcapacity );
                break;
            case 18 :
                dirty += "`guildcoin`=";
                dirty += utils::Integer::toString( m_db_guildcoin );
                break;
            case 19 :
                dirty += "`opensystems`=";
                dirty += "'?'";
                escapevalues.push_back( m_db_opensystems );
                break;
        }
    }

    int32_t len = 32 + 8 + dirty.size() + 20 + 32;

    sqlcmd.resize( len );
    len = std::snprintf( (char *)sqlcmd.data(), len-1,
        "UPDATE `CharBase` SET %s WHERE `roleid`=%" PRIu64 "", dirty.c_str(), m_db_roleid );
    sqlcmd.resize( len );

    return true;
}

bool CharBase_Data::remove( std::string & sqlcmd, std::vector<std::string> & escapevalues )
{
    uint32_t len = 32 + 8 + 20 + 32;

    sqlcmd.resize( len );
    len = std::snprintf( (char *)sqlcmd.data(), len-1,
        "DELETE FROM `CharBase` WHERE `roleid`=%" PRIu64 "", m_db_roleid );
    sqlcmd.resize( len );

    return true;
}

bool CharBase_Data::replace( std::string & sqlcmd, std::vector<std::string> & escapevalues )
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
                values += utils::Integer::toString( m_db_roleid );
                break;
            case 1 :
                fields += "`name`";
                values += "'?'";
                escapevalues.push_back( m_db_name );
                break;
            case 2 :
                fields += "`account`";
                values += "'?'";
                escapevalues.push_back( m_db_account );
                break;
            case 3 :
                fields += "`zone`";
                values += utils::Integer::toString( m_db_zone );
                break;
            case 4 :
                fields += "`createtime`";
                values += utils::Integer::toString( m_db_createtime );
                break;
            case 5 :
                fields += "`lastlogintime`";
                values += utils::Integer::toString( m_db_lastlogintime );
                break;
            case 6 :
                fields += "`lastlogouttime`";
                values += utils::Integer::toString( m_db_lastlogouttime );
                break;
            case 7 :
                fields += "`exp`";
                values += utils::Integer::toString( m_db_exp );
                break;
            case 8 :
                fields += "`money`";
                values += utils::Integer::toString( m_db_money );
                break;
            case 9 :
                fields += "`diamond`";
                values += utils::Integer::toString( m_db_diamond );
                break;
            case 10 :
                fields += "`gamecoin`";
                values += utils::Integer::toString( m_db_gamecoin );
                break;
            case 11 :
                fields += "`arenacoin`";
                values += utils::Integer::toString( m_db_arenacoin );
                break;
            case 12 :
                fields += "`strength`";
                values += utils::Integer::toString( m_db_strength );
                break;
            case 13 :
                fields += "`strengthtimestamp`";
                values += utils::Integer::toString( m_db_strengthtimestamp );
                break;
            case 14 :
                fields += "`vipexp`";
                values += utils::Integer::toString( m_db_vipexp );
                break;
            case 15 :
                fields += "`lastcity`";
                values += utils::Integer::toString( m_db_lastcity );
                break;
            case 16 :
                fields += "`lastposition`";
                values += "'?'";
                escapevalues.push_back( m_db_lastposition );
                break;
            case 17 :
                fields += "`totembagcapacity`";
                values += utils::Integer::toString( m_db_totembagcapacity );
                break;
            case 18 :
                fields += "`guildcoin`";
                values += utils::Integer::toString( m_db_guildcoin );
                break;
            case 19 :
                fields += "`opensystems`";
                values += "'?'";
                escapevalues.push_back( m_db_opensystems );
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
                    dirty += "`name`=";
                    dirty += "'?'";
                    escapevalues.push_back( m_db_name );
                    break;
                case 2 :
                    dirty += "`account`=";
                    dirty += "'?'";
                    escapevalues.push_back( m_db_account );
                    break;
                case 3 :
                    dirty += "`zone`=";
                    dirty += utils::Integer::toString( m_db_zone );
                    break;
                case 4 :
                    dirty += "`createtime`=";
                    dirty += utils::Integer::toString( m_db_createtime );
                    break;
                case 5 :
                    dirty += "`lastlogintime`=";
                    dirty += utils::Integer::toString( m_db_lastlogintime );
                    break;
                case 6 :
                    dirty += "`lastlogouttime`=";
                    dirty += utils::Integer::toString( m_db_lastlogouttime );
                    break;
                case 7 :
                    dirty += "`exp`=";
                    dirty += utils::Integer::toString( m_db_exp );
                    break;
                case 8 :
                    dirty += "`money`=";
                    dirty += utils::Integer::toString( m_db_money );
                    break;
                case 9 :
                    dirty += "`diamond`=";
                    dirty += utils::Integer::toString( m_db_diamond );
                    break;
                case 10 :
                    dirty += "`gamecoin`=";
                    dirty += utils::Integer::toString( m_db_gamecoin );
                    break;
                case 11 :
                    dirty += "`arenacoin`=";
                    dirty += utils::Integer::toString( m_db_arenacoin );
                    break;
                case 12 :
                    dirty += "`strength`=";
                    dirty += utils::Integer::toString( m_db_strength );
                    break;
                case 13 :
                    dirty += "`strengthtimestamp`=";
                    dirty += utils::Integer::toString( m_db_strengthtimestamp );
                    break;
                case 14 :
                    dirty += "`vipexp`=";
                    dirty += utils::Integer::toString( m_db_vipexp );
                    break;
                case 15 :
                    dirty += "`lastcity`=";
                    dirty += utils::Integer::toString( m_db_lastcity );
                    break;
                case 16 :
                    dirty += "`lastposition`=";
                    dirty += "'?'";
                    escapevalues.push_back( m_db_lastposition );
                    break;
                case 17 :
                    dirty += "`totembagcapacity`=";
                    dirty += utils::Integer::toString( m_db_totembagcapacity );
                    break;
                case 18 :
                    dirty += "`guildcoin`=";
                    dirty += utils::Integer::toString( m_db_guildcoin );
                    break;
                case 19 :
                    dirty += "`opensystems`=";
                    dirty += "'?'";
                    escapevalues.push_back( m_db_opensystems );
                    break;
            }
        }
    }

    uint32_t len = 64 + 8 + fields.size() + values.size() + dirty.size();

    sqlcmd.resize( len );
    len = std::snprintf( (char *)sqlcmd.data(), len-1,
        "INSERT INTO `CharBase` (%s) VALUES (%s) ON DUPLICATE KEY UPDATE %s", fields.c_str(), values.c_str(), dirty.c_str() );
    sqlcmd.resize(len);

    return true;
}

Slice CharBase_Data::encode( uint8_t op )
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
                    pack.encode( m_db_name );
                    break;
                case 2 :
                    ++nfields;
                    pack.encode( i );
                    pack.encode( m_db_account );
                    break;
                case 3 :
                    ++nfields;
                    pack.encode( i );
                    pack.encode( m_db_zone );
                    break;
                case 4 :
                    ++nfields;
                    pack.encode( i );
                    pack.encode( m_db_createtime );
                    break;
                case 5 :
                    ++nfields;
                    pack.encode( i );
                    pack.encode( m_db_lastlogintime );
                    break;
                case 6 :
                    ++nfields;
                    pack.encode( i );
                    pack.encode( m_db_lastlogouttime );
                    break;
                case 7 :
                    ++nfields;
                    pack.encode( i );
                    pack.encode( m_db_exp );
                    break;
                case 8 :
                    ++nfields;
                    pack.encode( i );
                    pack.encode( m_db_money );
                    break;
                case 9 :
                    ++nfields;
                    pack.encode( i );
                    pack.encode( m_db_diamond );
                    break;
                case 10 :
                    ++nfields;
                    pack.encode( i );
                    pack.encode( m_db_gamecoin );
                    break;
                case 11 :
                    ++nfields;
                    pack.encode( i );
                    pack.encode( m_db_arenacoin );
                    break;
                case 12 :
                    ++nfields;
                    pack.encode( i );
                    pack.encode( m_db_strength );
                    break;
                case 13 :
                    ++nfields;
                    pack.encode( i );
                    pack.encode( m_db_strengthtimestamp );
                    break;
                case 14 :
                    ++nfields;
                    pack.encode( i );
                    pack.encode( m_db_vipexp );
                    break;
                case 15 :
                    ++nfields;
                    pack.encode( i );
                    pack.encode( m_db_lastcity );
                    break;
                case 16 :
                    ++nfields;
                    pack.encode( i );
                    pack.encode( m_db_lastposition );
                    break;
                case 17 :
                    ++nfields;
                    pack.encode( i );
                    pack.encode( m_db_totembagcapacity );
                    break;
                case 18 :
                    ++nfields;
                    pack.encode( i );
                    pack.encode( m_db_guildcoin );
                    break;
                case 19 :
                    ++nfields;
                    pack.encode( i );
                    pack.encode( m_db_opensystems );
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

bool CharBase_Data::decode( const Slice & result, uint8_t op )
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
                unpack.decode( m_db_name );
                break;
            case 2 :
                m_dirty[2] = 1;
                unpack.decode( m_db_account );
                break;
            case 3 :
                m_dirty[3] = 1;
                unpack.decode( m_db_zone );
                break;
            case 4 :
                m_dirty[4] = 1;
                unpack.decode( m_db_createtime );
                break;
            case 5 :
                m_dirty[5] = 1;
                unpack.decode( m_db_lastlogintime );
                break;
            case 6 :
                m_dirty[6] = 1;
                unpack.decode( m_db_lastlogouttime );
                break;
            case 7 :
                m_dirty[7] = 1;
                unpack.decode( m_db_exp );
                break;
            case 8 :
                m_dirty[8] = 1;
                unpack.decode( m_db_money );
                break;
            case 9 :
                m_dirty[9] = 1;
                unpack.decode( m_db_diamond );
                break;
            case 10 :
                m_dirty[10] = 1;
                unpack.decode( m_db_gamecoin );
                break;
            case 11 :
                m_dirty[11] = 1;
                unpack.decode( m_db_arenacoin );
                break;
            case 12 :
                m_dirty[12] = 1;
                unpack.decode( m_db_strength );
                break;
            case 13 :
                m_dirty[13] = 1;
                unpack.decode( m_db_strengthtimestamp );
                break;
            case 14 :
                m_dirty[14] = 1;
                unpack.decode( m_db_vipexp );
                break;
            case 15 :
                m_dirty[15] = 1;
                unpack.decode( m_db_lastcity );
                break;
            case 16 :
                m_dirty[16] = 1;
                unpack.decode( m_db_lastposition );
                break;
            case 17 :
                m_dirty[17] = 1;
                unpack.decode( m_db_totembagcapacity );
                break;
            case 18 :
                m_dirty[18] = 1;
                unpack.decode( m_db_guildcoin );
                break;
            case 19 :
                m_dirty[19] = 1;
                unpack.decode( m_db_opensystems );
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

void CharBase_Data::merge( const ISQLData * data )
{
    assert( data->tablename() == TABLENAME );
    const CharBase_Data * d = static_cast<const CharBase_Data *>(data);

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
                set_db_name( d->get_db_name() );
                break;
            case 2 :
                m_dirty[2] = 1;
                set_db_account( d->get_db_account() );
                break;
            case 3 :
                m_dirty[3] = 1;
                set_db_zone( d->get_db_zone() );
                break;
            case 4 :
                m_dirty[4] = 1;
                set_db_createtime( d->get_db_createtime() );
                break;
            case 5 :
                m_dirty[5] = 1;
                set_db_lastlogintime( d->get_db_lastlogintime() );
                break;
            case 6 :
                m_dirty[6] = 1;
                set_db_lastlogouttime( d->get_db_lastlogouttime() );
                break;
            case 7 :
                m_dirty[7] = 1;
                set_db_exp( d->get_db_exp() );
                break;
            case 8 :
                m_dirty[8] = 1;
                set_db_money( d->get_db_money() );
                break;
            case 9 :
                m_dirty[9] = 1;
                set_db_diamond( d->get_db_diamond() );
                break;
            case 10 :
                m_dirty[10] = 1;
                set_db_gamecoin( d->get_db_gamecoin() );
                break;
            case 11 :
                m_dirty[11] = 1;
                set_db_arenacoin( d->get_db_arenacoin() );
                break;
            case 12 :
                m_dirty[12] = 1;
                set_db_strength( d->get_db_strength() );
                break;
            case 13 :
                m_dirty[13] = 1;
                set_db_strengthtimestamp( d->get_db_strengthtimestamp() );
                break;
            case 14 :
                m_dirty[14] = 1;
                set_db_vipexp( d->get_db_vipexp() );
                break;
            case 15 :
                m_dirty[15] = 1;
                set_db_lastcity( d->get_db_lastcity() );
                break;
            case 16 :
                m_dirty[16] = 1;
                set_db_lastposition( d->get_db_lastposition() );
                break;
            case 17 :
                m_dirty[17] = 1;
                set_db_totembagcapacity( d->get_db_totembagcapacity() );
                break;
            case 18 :
                m_dirty[18] = 1;
                set_db_guildcoin( d->get_db_guildcoin() );
                break;
            case 19 :
                m_dirty[19] = 1;
                set_db_opensystems( d->get_db_opensystems() );
                break;
        }
    }
}

bool CharBase_Data::match( const ISQLData * data ) const
{
    assert( data->tablename() == TABLENAME );
    const CharBase_Data * d = static_cast<const CharBase_Data *>(data);

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
                if ( get_db_name() != d->get_db_name() ) return false;
                break;
            case 2 :
                if ( get_db_account() != d->get_db_account() ) return false;
                break;
            case 3 :
                if ( get_db_zone() != d->get_db_zone() ) return false;
                break;
            case 4 :
                if ( get_db_createtime() != d->get_db_createtime() ) return false;
                break;
            case 5 :
                if ( get_db_lastlogintime() != d->get_db_lastlogintime() ) return false;
                break;
            case 6 :
                if ( get_db_lastlogouttime() != d->get_db_lastlogouttime() ) return false;
                break;
            case 7 :
                if ( get_db_exp() != d->get_db_exp() ) return false;
                break;
            case 8 :
                if ( get_db_money() != d->get_db_money() ) return false;
                break;
            case 9 :
                if ( get_db_diamond() != d->get_db_diamond() ) return false;
                break;
            case 10 :
                if ( get_db_gamecoin() != d->get_db_gamecoin() ) return false;
                break;
            case 11 :
                if ( get_db_arenacoin() != d->get_db_arenacoin() ) return false;
                break;
            case 12 :
                if ( get_db_strength() != d->get_db_strength() ) return false;
                break;
            case 13 :
                if ( get_db_strengthtimestamp() != d->get_db_strengthtimestamp() ) return false;
                break;
            case 14 :
                if ( get_db_vipexp() != d->get_db_vipexp() ) return false;
                break;
            case 15 :
                if ( get_db_lastcity() != d->get_db_lastcity() ) return false;
                break;
            case 16 :
                if ( get_db_lastposition() != d->get_db_lastposition() ) return false;
                break;
            case 17 :
                if ( get_db_totembagcapacity() != d->get_db_totembagcapacity() ) return false;
                break;
            case 18 :
                if ( get_db_guildcoin() != d->get_db_guildcoin() ) return false;
                break;
            case 19 :
                if ( get_db_opensystems() != d->get_db_opensystems() ) return false;
                break;
        }
    }
    return true;
}

bool CharBase_Data::parse( const Slices & result )
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
                m_db_name = result[i].ToString();
                break;
            case 2 :
                m_db_account = result[i].ToString();
                break;
            case 3 :
                m_db_zone = (uint16_t)std::atoi( result[i].ToString().c_str() );
                break;
            case 4 :
                m_db_createtime = (uint64_t)std::atoll( result[i].ToString().c_str() );
                break;
            case 5 :
                m_db_lastlogintime = (int64_t)std::atoll( result[i].ToString().c_str() );
                break;
            case 6 :
                m_db_lastlogouttime = (int64_t)std::atoll( result[i].ToString().c_str() );
                break;
            case 7 :
                m_db_exp = (int64_t)std::atoll( result[i].ToString().c_str() );
                break;
            case 8 :
                m_db_money = (int64_t)std::atoll( result[i].ToString().c_str() );
                break;
            case 9 :
                m_db_diamond = (int64_t)std::atoll( result[i].ToString().c_str() );
                break;
            case 10 :
                m_db_gamecoin = (int64_t)std::atoll( result[i].ToString().c_str() );
                break;
            case 11 :
                m_db_arenacoin = (int64_t)std::atoll( result[i].ToString().c_str() );
                break;
            case 12 :
                m_db_strength = (int64_t)std::atoll( result[i].ToString().c_str() );
                break;
            case 13 :
                m_db_strengthtimestamp = (int64_t)std::atoll( result[i].ToString().c_str() );
                break;
            case 14 :
                m_db_vipexp = (int64_t)std::atoll( result[i].ToString().c_str() );
                break;
            case 15 :
                m_db_lastcity = (uint32_t)std::atoi( result[i].ToString().c_str() );
                break;
            case 16 :
                m_db_lastposition = result[i].ToString();
                break;
            case 17 :
                m_db_totembagcapacity = (int32_t)std::atoi( result[i].ToString().c_str() );
                break;
            case 18 :
                m_db_guildcoin = (int64_t)std::atoll( result[i].ToString().c_str() );
                break;
            case 19 :
                m_db_opensystems = result[i].ToString();
                break;
        }
    }

    return true;
}

void CharBase_Data::registering( kaguya::State * state )
{
    (*state)["CharBase_Data"].setClass( kaguya::UserdataMetatable<CharBase_Data, ISQLData>()
        .setConstructors<CharBase_Data(), CharBase_Data(uint64_t)>()
        .addFunction( "get_db_roleid", &CharBase_Data::get_db_roleid )
        .addFunction( "set_db_roleid", &CharBase_Data::set_db_roleid )
        .addFunction( "get_db_name", &CharBase_Data::get_db_name )
        .addFunction( "set_db_name", &CharBase_Data::set_db_name )
        .addFunction( "get_db_account", &CharBase_Data::get_db_account )
        .addFunction( "set_db_account", &CharBase_Data::set_db_account )
        .addFunction( "get_db_zone", &CharBase_Data::get_db_zone )
        .addFunction( "set_db_zone", &CharBase_Data::set_db_zone )
        .addFunction( "get_db_createtime", &CharBase_Data::get_db_createtime )
        .addFunction( "set_db_createtime", &CharBase_Data::set_db_createtime )
        .addFunction( "get_db_lastlogintime", &CharBase_Data::get_db_lastlogintime )
        .addFunction( "set_db_lastlogintime", &CharBase_Data::set_db_lastlogintime )
        .addFunction( "get_db_lastlogouttime", &CharBase_Data::get_db_lastlogouttime )
        .addFunction( "set_db_lastlogouttime", &CharBase_Data::set_db_lastlogouttime )
        .addFunction( "get_db_exp", &CharBase_Data::get_db_exp )
        .addFunction( "set_db_exp", &CharBase_Data::set_db_exp )
        .addFunction( "get_db_money", &CharBase_Data::get_db_money )
        .addFunction( "set_db_money", &CharBase_Data::set_db_money )
        .addFunction( "get_db_diamond", &CharBase_Data::get_db_diamond )
        .addFunction( "set_db_diamond", &CharBase_Data::set_db_diamond )
        .addFunction( "get_db_gamecoin", &CharBase_Data::get_db_gamecoin )
        .addFunction( "set_db_gamecoin", &CharBase_Data::set_db_gamecoin )
        .addFunction( "get_db_arenacoin", &CharBase_Data::get_db_arenacoin )
        .addFunction( "set_db_arenacoin", &CharBase_Data::set_db_arenacoin )
        .addFunction( "get_db_strength", &CharBase_Data::get_db_strength )
        .addFunction( "set_db_strength", &CharBase_Data::set_db_strength )
        .addFunction( "get_db_strengthtimestamp", &CharBase_Data::get_db_strengthtimestamp )
        .addFunction( "set_db_strengthtimestamp", &CharBase_Data::set_db_strengthtimestamp )
        .addFunction( "get_db_vipexp", &CharBase_Data::get_db_vipexp )
        .addFunction( "set_db_vipexp", &CharBase_Data::set_db_vipexp )
        .addFunction( "get_db_lastcity", &CharBase_Data::get_db_lastcity )
        .addFunction( "set_db_lastcity", &CharBase_Data::set_db_lastcity )
        .addFunction( "get_db_lastposition", &CharBase_Data::get_db_lastposition )
        .addFunction( "set_db_lastposition", &CharBase_Data::set_db_lastposition )
        .addFunction( "get_db_totembagcapacity", &CharBase_Data::get_db_totembagcapacity )
        .addFunction( "set_db_totembagcapacity", &CharBase_Data::set_db_totembagcapacity )
        .addFunction( "get_db_guildcoin", &CharBase_Data::get_db_guildcoin )
        .addFunction( "set_db_guildcoin", &CharBase_Data::set_db_guildcoin )
        .addFunction( "get_db_opensystems", &CharBase_Data::get_db_opensystems )
        .addFunction( "set_db_opensystems", &CharBase_Data::set_db_opensystems )
        .addStaticFunction( "convert", [] ( ISQLData * data ) -> CharBase_Data * { return static_cast<CharBase_Data *>( data ); } )
    );
}

bool CharBase_Data::invoke( kaguya::State * state, const std::string & script )
{
    (*state)["CharBase"] = this;
    state->dostring( script.c_str() );
    (*state)["CharBase"] = nullptr;
    return true;
}

void CharBase_Data::copyfrom( const CharBase_Data & data )
{
    // Field: name, Index: 1
    m_dirty[1] = 1;
    m_db_name = data.get_db_name();
    // Field: account, Index: 2
    m_dirty[2] = 1;
    m_db_account = data.get_db_account();
    // Field: zone, Index: 3
    m_dirty[3] = 1;
    m_db_zone = data.get_db_zone();
    // Field: createtime, Index: 4
    m_dirty[4] = 1;
    m_db_createtime = data.get_db_createtime();
    // Field: lastlogintime, Index: 5
    m_dirty[5] = 1;
    m_db_lastlogintime = data.get_db_lastlogintime();
    // Field: lastlogouttime, Index: 6
    m_dirty[6] = 1;
    m_db_lastlogouttime = data.get_db_lastlogouttime();
    // Field: exp, Index: 7
    m_dirty[7] = 1;
    m_db_exp = data.get_db_exp();
    // Field: money, Index: 8
    m_dirty[8] = 1;
    m_db_money = data.get_db_money();
    // Field: diamond, Index: 9
    m_dirty[9] = 1;
    m_db_diamond = data.get_db_diamond();
    // Field: gamecoin, Index: 10
    m_dirty[10] = 1;
    m_db_gamecoin = data.get_db_gamecoin();
    // Field: arenacoin, Index: 11
    m_dirty[11] = 1;
    m_db_arenacoin = data.get_db_arenacoin();
    // Field: strength, Index: 12
    m_dirty[12] = 1;
    m_db_strength = data.get_db_strength();
    // Field: strengthtimestamp, Index: 13
    m_dirty[13] = 1;
    m_db_strengthtimestamp = data.get_db_strengthtimestamp();
    // Field: vipexp, Index: 14
    m_dirty[14] = 1;
    m_db_vipexp = data.get_db_vipexp();
    // Field: lastcity, Index: 15
    m_dirty[15] = 1;
    m_db_lastcity = data.get_db_lastcity();
    // Field: lastposition, Index: 16
    m_dirty[16] = 1;
    m_db_lastposition = data.get_db_lastposition();
    // Field: totembagcapacity, Index: 17
    m_dirty[17] = 1;
    m_db_totembagcapacity = data.get_db_totembagcapacity();
    // Field: guildcoin, Index: 18
    m_dirty[18] = 1;
    m_db_guildcoin = data.get_db_guildcoin();
    // Field: opensystems, Index: 19
    m_dirty[19] = 1;
    m_db_opensystems = data.get_db_opensystems();
}

}
