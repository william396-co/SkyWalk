
#include <cstdio>
#include <cassert>
#include <cinttypes>
#include <cstdlib>

#include "lua/kaguya.hpp"
#include "utils/integer.h"
#include "utils/streambuf.h"

#include "CharBrief.hpp"

namespace data
{

// Tablename
const std::string CharBrief_Data::TABLENAME = "CharBrief";

// VARCHAR Length Limit
const size_t CharBrief_Data::MAX_DB_ACCOUNT_LENGTH = 128;
const size_t CharBrief_Data::MAX_DB_STOREACCOUNT_LENGTH = 128;
const size_t CharBrief_Data::MAX_DB_NAME_LENGTH = 128;
const size_t CharBrief_Data::MAX_DB_LASTERCITY_LENGTH = 64;
const size_t CharBrief_Data::MAX_DB_BANNEDREASON_LENGTH = 512;
const size_t CharBrief_Data::MAX_DB_OPENSYSTEMS_LENGTH = 1024;
const size_t CharBrief_Data::MAX_DB_MAINPROGRESS_LENGTH = 256;

CharBrief_Data::CharBrief_Data()
{
    clean();
    // Fields
    m_db_roleid = 0;
    m_db_storeaccount = "";
    m_db_status = 1;
    m_db_name = "";
    m_db_zone = 0;
    m_db_createtime = 0;
    m_db_level = 1;
    m_db_viplevel = 0;
    m_db_avatar = 0;
    m_db_battlepoint = 0;
    m_db_lastercity = "";
    m_db_lastlogintime = 0;
    m_db_lastlogouttime = 0;
    m_db_bannedtime = 0;
    m_db_bannedreason = "";
    m_db_shutuptime = 0;
    m_db_opensystems = "";
    m_db_mainprogress = "";
    m_db_friendscount = 0;
    m_db_settings = 0;
    m_db_arenastatus = 0;
}

CharBrief_Data::CharBrief_Data( uint64_t roleid, const std::string & account )
{
    clean();
    // Fields
    m_db_roleid = roleid;
    m_db_account = account;
    m_db_storeaccount = "";
    m_db_status = 1;
    m_db_name = "";
    m_db_zone = 0;
    m_db_createtime = 0;
    m_db_level = 1;
    m_db_viplevel = 0;
    m_db_avatar = 0;
    m_db_battlepoint = 0;
    m_db_lastercity = "";
    m_db_lastlogintime = 0;
    m_db_lastlogouttime = 0;
    m_db_bannedtime = 0;
    m_db_bannedreason = "";
    m_db_shutuptime = 0;
    m_db_opensystems = "";
    m_db_mainprogress = "";
    m_db_friendscount = 0;
    m_db_settings = 0;
    m_db_arenastatus = 0;
}

CharBrief_Data::~CharBrief_Data()
{
    m_db_roleid = 0;
    m_db_account.clear();
    m_db_storeaccount.clear();
    m_db_status = 0;
    m_db_name.clear();
    m_db_zone = 0;
    m_db_createtime = 0;
    m_db_level = 0;
    m_db_viplevel = 0;
    m_db_avatar = 0;
    m_db_battlepoint = 0;
    m_db_lastercity.clear();
    m_db_lastlogintime = 0;
    m_db_lastlogouttime = 0;
    m_db_bannedtime = 0;
    m_db_bannedreason.clear();
    m_db_shutuptime = 0;
    m_db_opensystems.clear();
    m_db_mainprogress.clear();
    m_db_friendscount = 0;
    m_db_settings = 0;
    m_db_arenastatus = 0;
}

void CharBrief_Data::clean()
{
    m_method = DBMethod::None;
    bzero( &m_dirty, sizeof(m_dirty) );
}

ISQLData * CharBrief_Data::clone( bool isfull ) const
{
    CharBrief_Data * data = new CharBrief_Data();

    if ( data != nullptr && isfull )
    {
        data->m_method = m_method;
        memcpy( data->m_dirty, m_dirty, NFIELDS );

        data->m_db_roleid = m_db_roleid;
        data->m_db_account = m_db_account;
        data->m_db_storeaccount = m_db_storeaccount;
        data->m_db_status = m_db_status;
        data->m_db_name = m_db_name;
        data->m_db_zone = m_db_zone;
        data->m_db_createtime = m_db_createtime;
        data->m_db_level = m_db_level;
        data->m_db_viplevel = m_db_viplevel;
        data->m_db_avatar = m_db_avatar;
        data->m_db_battlepoint = m_db_battlepoint;
        data->m_db_lastercity = m_db_lastercity;
        data->m_db_lastlogintime = m_db_lastlogintime;
        data->m_db_lastlogouttime = m_db_lastlogouttime;
        data->m_db_bannedtime = m_db_bannedtime;
        data->m_db_bannedreason = m_db_bannedreason;
        data->m_db_shutuptime = m_db_shutuptime;
        data->m_db_opensystems = m_db_opensystems;
        data->m_db_mainprogress = m_db_mainprogress;
        data->m_db_friendscount = m_db_friendscount;
        data->m_db_settings = m_db_settings;
        data->m_db_arenastatus = m_db_arenastatus;
    }

    return data;
}

bool CharBrief_Data::isDirty() const
{
    for ( uint16_t i = 0; i < NFIELDS; ++i )
    {
        if ( m_dirty[i] != 0 ) return true;
    }

    return false;
}

bool CharBrief_Data::isKeydirty() const
{
    return m_dirty[0] == 1 || m_dirty[1] == 1;
}

void CharBrief_Data::keystring( std::string & k ) const
{
    k = utils::Integer::toString( m_db_roleid );
}

void CharBrief_Data::indexstring( uint8_t op, std::string & k ) const
{
    if ( likely(op == eCodec_All) )
    {
        k = m_db_account;
    }
    else
    {
        if ( m_dirty[1] == 1 )
        {
            if ( !k.empty() ) k += "#";
            k += m_db_account;
        }
    }
}

void CharBrief_Data::autoincrease( uint8_t source, const Slice & value )
{
}

bool CharBrief_Data::query( std::string & sqlcmd, std::vector<std::string> & escapevalues, bool all )
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
                    fields += "`account`=";
                    fields += "'?'";
                    escapevalues.push_back( m_db_account );
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
                    fields += "`account`=";
                    fields += "'?'";
                    escapevalues.push_back( m_db_account );
                    break;
            }
        }
    }

    if ( fields.empty() )
    {
        sqlcmd = "SELECT `roleid`,`account`,`storeaccount`,`status`,`name`,`zone`,`createtime`,`level`,`viplevel`,`avatar`,`battlepoint`,`lastercity`,`lastlogintime`,`lastlogouttime`,`bannedtime`,`bannedreason`,`shutuptime`,`opensystems`,`mainprogress`,`friendscount`,`settings`,`arenastatus` FROM `CharBrief`";
    }
    else
    {
        uint32_t len = 32 + 267 + 0 + 9 + fields.size();
        sqlcmd.resize( len );
        len = std::snprintf(
            (char *)sqlcmd.data(), len-1,
            "SELECT `roleid`,`account`,`storeaccount`,`status`,`name`,`zone`,`createtime`,`level`,`viplevel`,`avatar`,`battlepoint`,`lastercity`,`lastlogintime`,`lastlogouttime`,`bannedtime`,`bannedreason`,`shutuptime`,`opensystems`,`mainprogress`,`friendscount`,`settings`,`arenastatus` FROM `CharBrief` WHERE %s", fields.c_str() );
        sqlcmd.resize( len );
    }

    return true;
}

bool CharBrief_Data::insert( std::string & sqlcmd, std::vector<std::string> & escapevalues )
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
                fields += "`account`";
                values += "'?'";
                escapevalues.push_back( m_db_account );
                break;
            case 2 :
                fields += "`storeaccount`";
                values += "'?'";
                escapevalues.push_back( m_db_storeaccount );
                break;
            case 3 :
                fields += "`status`";
                values += utils::Integer::toString( m_db_status );
                break;
            case 4 :
                fields += "`name`";
                values += "'?'";
                escapevalues.push_back( m_db_name );
                break;
            case 5 :
                fields += "`zone`";
                values += utils::Integer::toString( m_db_zone );
                break;
            case 6 :
                fields += "`createtime`";
                values += utils::Integer::toString( m_db_createtime );
                break;
            case 7 :
                fields += "`level`";
                values += utils::Integer::toString( m_db_level );
                break;
            case 8 :
                fields += "`viplevel`";
                values += utils::Integer::toString( m_db_viplevel );
                break;
            case 9 :
                fields += "`avatar`";
                values += utils::Integer::toString( m_db_avatar );
                break;
            case 10 :
                fields += "`battlepoint`";
                values += utils::Integer::toString( m_db_battlepoint );
                break;
            case 11 :
                fields += "`lastercity`";
                values += "'?'";
                escapevalues.push_back( m_db_lastercity );
                break;
            case 12 :
                fields += "`lastlogintime`";
                values += utils::Integer::toString( m_db_lastlogintime );
                break;
            case 13 :
                fields += "`lastlogouttime`";
                values += utils::Integer::toString( m_db_lastlogouttime );
                break;
            case 14 :
                fields += "`bannedtime`";
                values += utils::Integer::toString( m_db_bannedtime );
                break;
            case 15 :
                fields += "`bannedreason`";
                values += "'?'";
                escapevalues.push_back( m_db_bannedreason );
                break;
            case 16 :
                fields += "`shutuptime`";
                values += utils::Integer::toString( m_db_shutuptime );
                break;
            case 17 :
                fields += "`opensystems`";
                values += "'?'";
                escapevalues.push_back( m_db_opensystems );
                break;
            case 18 :
                fields += "`mainprogress`";
                values += "'?'";
                escapevalues.push_back( m_db_mainprogress );
                break;
            case 19 :
                fields += "`friendscount`";
                values += utils::Integer::toString( m_db_friendscount );
                break;
            case 20 :
                fields += "`settings`";
                values += utils::Integer::toString( m_db_settings );
                break;
            case 21 :
                fields += "`arenastatus`";
                values += utils::Integer::toString( m_db_arenastatus );
                break;
        }
    }

    uint32_t len = 32 + 9 + fields.size() + values.size();

    sqlcmd.resize( len );
    len = std::snprintf( (char *)sqlcmd.data(), len-1,
        "INSERT INTO `CharBrief` (%s) VALUES (%s)", fields.c_str(), values.c_str() );
    sqlcmd.resize(len);

    return true;
}

bool CharBrief_Data::update( std::string & sqlcmd, std::vector<std::string> & escapevalues )
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
                dirty += "`account`=";
                dirty += "'?'";
                escapevalues.push_back( m_db_account );
                break;
            case 2 :
                dirty += "`storeaccount`=";
                dirty += "'?'";
                escapevalues.push_back( m_db_storeaccount );
                break;
            case 3 :
                dirty += "`status`=";
                dirty += utils::Integer::toString( m_db_status );
                break;
            case 4 :
                dirty += "`name`=";
                dirty += "'?'";
                escapevalues.push_back( m_db_name );
                break;
            case 5 :
                dirty += "`zone`=";
                dirty += utils::Integer::toString( m_db_zone );
                break;
            case 6 :
                dirty += "`createtime`=";
                dirty += utils::Integer::toString( m_db_createtime );
                break;
            case 7 :
                dirty += "`level`=";
                dirty += utils::Integer::toString( m_db_level );
                break;
            case 8 :
                dirty += "`viplevel`=";
                dirty += utils::Integer::toString( m_db_viplevel );
                break;
            case 9 :
                dirty += "`avatar`=";
                dirty += utils::Integer::toString( m_db_avatar );
                break;
            case 10 :
                dirty += "`battlepoint`=";
                dirty += utils::Integer::toString( m_db_battlepoint );
                break;
            case 11 :
                dirty += "`lastercity`=";
                dirty += "'?'";
                escapevalues.push_back( m_db_lastercity );
                break;
            case 12 :
                dirty += "`lastlogintime`=";
                dirty += utils::Integer::toString( m_db_lastlogintime );
                break;
            case 13 :
                dirty += "`lastlogouttime`=";
                dirty += utils::Integer::toString( m_db_lastlogouttime );
                break;
            case 14 :
                dirty += "`bannedtime`=";
                dirty += utils::Integer::toString( m_db_bannedtime );
                break;
            case 15 :
                dirty += "`bannedreason`=";
                dirty += "'?'";
                escapevalues.push_back( m_db_bannedreason );
                break;
            case 16 :
                dirty += "`shutuptime`=";
                dirty += utils::Integer::toString( m_db_shutuptime );
                break;
            case 17 :
                dirty += "`opensystems`=";
                dirty += "'?'";
                escapevalues.push_back( m_db_opensystems );
                break;
            case 18 :
                dirty += "`mainprogress`=";
                dirty += "'?'";
                escapevalues.push_back( m_db_mainprogress );
                break;
            case 19 :
                dirty += "`friendscount`=";
                dirty += utils::Integer::toString( m_db_friendscount );
                break;
            case 20 :
                dirty += "`settings`=";
                dirty += utils::Integer::toString( m_db_settings );
                break;
            case 21 :
                dirty += "`arenastatus`=";
                dirty += utils::Integer::toString( m_db_arenastatus );
                break;
        }
    }

    int32_t len = 32 + 9 + dirty.size() + 20 + 32;

    sqlcmd.resize( len );
    len = std::snprintf( (char *)sqlcmd.data(), len-1,
        "UPDATE `CharBrief` SET %s WHERE `roleid`=%" PRIu64 "", dirty.c_str(), m_db_roleid );
    sqlcmd.resize( len );

    return true;
}

bool CharBrief_Data::remove( std::string & sqlcmd, std::vector<std::string> & escapevalues )
{
    uint32_t len = 32 + 9 + 20 + 32;

    sqlcmd.resize( len );
    len = std::snprintf( (char *)sqlcmd.data(), len-1,
        "DELETE FROM `CharBrief` WHERE `roleid`=%" PRIu64 "", m_db_roleid );
    sqlcmd.resize( len );

    return true;
}

bool CharBrief_Data::replace( std::string & sqlcmd, std::vector<std::string> & escapevalues )
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
                fields += "`account`";
                values += "'?'";
                escapevalues.push_back( m_db_account );
                break;
            case 2 :
                fields += "`storeaccount`";
                values += "'?'";
                escapevalues.push_back( m_db_storeaccount );
                break;
            case 3 :
                fields += "`status`";
                values += utils::Integer::toString( m_db_status );
                break;
            case 4 :
                fields += "`name`";
                values += "'?'";
                escapevalues.push_back( m_db_name );
                break;
            case 5 :
                fields += "`zone`";
                values += utils::Integer::toString( m_db_zone );
                break;
            case 6 :
                fields += "`createtime`";
                values += utils::Integer::toString( m_db_createtime );
                break;
            case 7 :
                fields += "`level`";
                values += utils::Integer::toString( m_db_level );
                break;
            case 8 :
                fields += "`viplevel`";
                values += utils::Integer::toString( m_db_viplevel );
                break;
            case 9 :
                fields += "`avatar`";
                values += utils::Integer::toString( m_db_avatar );
                break;
            case 10 :
                fields += "`battlepoint`";
                values += utils::Integer::toString( m_db_battlepoint );
                break;
            case 11 :
                fields += "`lastercity`";
                values += "'?'";
                escapevalues.push_back( m_db_lastercity );
                break;
            case 12 :
                fields += "`lastlogintime`";
                values += utils::Integer::toString( m_db_lastlogintime );
                break;
            case 13 :
                fields += "`lastlogouttime`";
                values += utils::Integer::toString( m_db_lastlogouttime );
                break;
            case 14 :
                fields += "`bannedtime`";
                values += utils::Integer::toString( m_db_bannedtime );
                break;
            case 15 :
                fields += "`bannedreason`";
                values += "'?'";
                escapevalues.push_back( m_db_bannedreason );
                break;
            case 16 :
                fields += "`shutuptime`";
                values += utils::Integer::toString( m_db_shutuptime );
                break;
            case 17 :
                fields += "`opensystems`";
                values += "'?'";
                escapevalues.push_back( m_db_opensystems );
                break;
            case 18 :
                fields += "`mainprogress`";
                values += "'?'";
                escapevalues.push_back( m_db_mainprogress );
                break;
            case 19 :
                fields += "`friendscount`";
                values += utils::Integer::toString( m_db_friendscount );
                break;
            case 20 :
                fields += "`settings`";
                values += utils::Integer::toString( m_db_settings );
                break;
            case 21 :
                fields += "`arenastatus`";
                values += utils::Integer::toString( m_db_arenastatus );
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
                    dirty += "`storeaccount`=";
                    dirty += "'?'";
                    escapevalues.push_back( m_db_storeaccount );
                    break;
                case 3 :
                    dirty += "`status`=";
                    dirty += utils::Integer::toString( m_db_status );
                    break;
                case 4 :
                    dirty += "`name`=";
                    dirty += "'?'";
                    escapevalues.push_back( m_db_name );
                    break;
                case 5 :
                    dirty += "`zone`=";
                    dirty += utils::Integer::toString( m_db_zone );
                    break;
                case 6 :
                    dirty += "`createtime`=";
                    dirty += utils::Integer::toString( m_db_createtime );
                    break;
                case 7 :
                    dirty += "`level`=";
                    dirty += utils::Integer::toString( m_db_level );
                    break;
                case 8 :
                    dirty += "`viplevel`=";
                    dirty += utils::Integer::toString( m_db_viplevel );
                    break;
                case 9 :
                    dirty += "`avatar`=";
                    dirty += utils::Integer::toString( m_db_avatar );
                    break;
                case 10 :
                    dirty += "`battlepoint`=";
                    dirty += utils::Integer::toString( m_db_battlepoint );
                    break;
                case 11 :
                    dirty += "`lastercity`=";
                    dirty += "'?'";
                    escapevalues.push_back( m_db_lastercity );
                    break;
                case 12 :
                    dirty += "`lastlogintime`=";
                    dirty += utils::Integer::toString( m_db_lastlogintime );
                    break;
                case 13 :
                    dirty += "`lastlogouttime`=";
                    dirty += utils::Integer::toString( m_db_lastlogouttime );
                    break;
                case 14 :
                    dirty += "`bannedtime`=";
                    dirty += utils::Integer::toString( m_db_bannedtime );
                    break;
                case 15 :
                    dirty += "`bannedreason`=";
                    dirty += "'?'";
                    escapevalues.push_back( m_db_bannedreason );
                    break;
                case 16 :
                    dirty += "`shutuptime`=";
                    dirty += utils::Integer::toString( m_db_shutuptime );
                    break;
                case 17 :
                    dirty += "`opensystems`=";
                    dirty += "'?'";
                    escapevalues.push_back( m_db_opensystems );
                    break;
                case 18 :
                    dirty += "`mainprogress`=";
                    dirty += "'?'";
                    escapevalues.push_back( m_db_mainprogress );
                    break;
                case 19 :
                    dirty += "`friendscount`=";
                    dirty += utils::Integer::toString( m_db_friendscount );
                    break;
                case 20 :
                    dirty += "`settings`=";
                    dirty += utils::Integer::toString( m_db_settings );
                    break;
                case 21 :
                    dirty += "`arenastatus`=";
                    dirty += utils::Integer::toString( m_db_arenastatus );
                    break;
            }
        }
    }

    uint32_t len = 64 + 9 + fields.size() + values.size() + dirty.size();

    sqlcmd.resize( len );
    len = std::snprintf( (char *)sqlcmd.data(), len-1,
        "INSERT INTO `CharBrief` (%s) VALUES (%s) ON DUPLICATE KEY UPDATE %s", fields.c_str(), values.c_str(), dirty.c_str() );
    sqlcmd.resize(len);

    return true;
}

Slice CharBrief_Data::encode( uint8_t op )
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
            pack.encode( m_db_account );
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
                    pack.encode( m_db_account );
                    break;
                case 2 :
                    ++nfields;
                    pack.encode( i );
                    pack.encode( m_db_storeaccount );
                    break;
                case 3 :
                    ++nfields;
                    pack.encode( i );
                    pack.encode( m_db_status );
                    break;
                case 4 :
                    ++nfields;
                    pack.encode( i );
                    pack.encode( m_db_name );
                    break;
                case 5 :
                    ++nfields;
                    pack.encode( i );
                    pack.encode( m_db_zone );
                    break;
                case 6 :
                    ++nfields;
                    pack.encode( i );
                    pack.encode( m_db_createtime );
                    break;
                case 7 :
                    ++nfields;
                    pack.encode( i );
                    pack.encode( m_db_level );
                    break;
                case 8 :
                    ++nfields;
                    pack.encode( i );
                    pack.encode( m_db_viplevel );
                    break;
                case 9 :
                    ++nfields;
                    pack.encode( i );
                    pack.encode( m_db_avatar );
                    break;
                case 10 :
                    ++nfields;
                    pack.encode( i );
                    pack.encode( m_db_battlepoint );
                    break;
                case 11 :
                    ++nfields;
                    pack.encode( i );
                    pack.encode( m_db_lastercity );
                    break;
                case 12 :
                    ++nfields;
                    pack.encode( i );
                    pack.encode( m_db_lastlogintime );
                    break;
                case 13 :
                    ++nfields;
                    pack.encode( i );
                    pack.encode( m_db_lastlogouttime );
                    break;
                case 14 :
                    ++nfields;
                    pack.encode( i );
                    pack.encode( m_db_bannedtime );
                    break;
                case 15 :
                    ++nfields;
                    pack.encode( i );
                    pack.encode( m_db_bannedreason );
                    break;
                case 16 :
                    ++nfields;
                    pack.encode( i );
                    pack.encode( m_db_shutuptime );
                    break;
                case 17 :
                    ++nfields;
                    pack.encode( i );
                    pack.encode( m_db_opensystems );
                    break;
                case 18 :
                    ++nfields;
                    pack.encode( i );
                    pack.encode( m_db_mainprogress );
                    break;
                case 19 :
                    ++nfields;
                    pack.encode( i );
                    pack.encode( m_db_friendscount );
                    break;
                case 20 :
                    ++nfields;
                    pack.encode( i );
                    pack.encode( m_db_settings );
                    break;
                case 21 :
                    ++nfields;
                    pack.encode( i );
                    pack.encode( m_db_arenastatus );
                    break;
            }
        }
    }

    // key fields
    if ( op != eCodec_Query )
    {
        pack.encode( m_db_roleid );
        pack.encode( m_db_account );
    }

    // fields number
    pack.reset();
    pack.encode( nfields );

    return pack.slice();
}

bool CharBrief_Data::decode( const Slice & result, uint8_t op )
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
                unpack.decode( m_db_account );
                break;
            case 2 :
                m_dirty[2] = 1;
                unpack.decode( m_db_storeaccount );
                break;
            case 3 :
                m_dirty[3] = 1;
                unpack.decode( m_db_status );
                break;
            case 4 :
                m_dirty[4] = 1;
                unpack.decode( m_db_name );
                break;
            case 5 :
                m_dirty[5] = 1;
                unpack.decode( m_db_zone );
                break;
            case 6 :
                m_dirty[6] = 1;
                unpack.decode( m_db_createtime );
                break;
            case 7 :
                m_dirty[7] = 1;
                unpack.decode( m_db_level );
                break;
            case 8 :
                m_dirty[8] = 1;
                unpack.decode( m_db_viplevel );
                break;
            case 9 :
                m_dirty[9] = 1;
                unpack.decode( m_db_avatar );
                break;
            case 10 :
                m_dirty[10] = 1;
                unpack.decode( m_db_battlepoint );
                break;
            case 11 :
                m_dirty[11] = 1;
                unpack.decode( m_db_lastercity );
                break;
            case 12 :
                m_dirty[12] = 1;
                unpack.decode( m_db_lastlogintime );
                break;
            case 13 :
                m_dirty[13] = 1;
                unpack.decode( m_db_lastlogouttime );
                break;
            case 14 :
                m_dirty[14] = 1;
                unpack.decode( m_db_bannedtime );
                break;
            case 15 :
                m_dirty[15] = 1;
                unpack.decode( m_db_bannedreason );
                break;
            case 16 :
                m_dirty[16] = 1;
                unpack.decode( m_db_shutuptime );
                break;
            case 17 :
                m_dirty[17] = 1;
                unpack.decode( m_db_opensystems );
                break;
            case 18 :
                m_dirty[18] = 1;
                unpack.decode( m_db_mainprogress );
                break;
            case 19 :
                m_dirty[19] = 1;
                unpack.decode( m_db_friendscount );
                break;
            case 20 :
                m_dirty[20] = 1;
                unpack.decode( m_db_settings );
                break;
            case 21 :
                m_dirty[21] = 1;
                unpack.decode( m_db_arenastatus );
                break;
        }
    }

    // keys
    if ( op != eCodec_Query )
    {
        unpack.decode( m_db_roleid );
        unpack.decode( m_db_account );
    }

    return true;
}

void CharBrief_Data::merge( const ISQLData * data )
{
    assert( data->tablename() == TABLENAME );
    const CharBrief_Data * d = static_cast<const CharBrief_Data *>(data);

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
                set_db_account( d->get_db_account() );
                break;
            case 2 :
                m_dirty[2] = 1;
                set_db_storeaccount( d->get_db_storeaccount() );
                break;
            case 3 :
                m_dirty[3] = 1;
                set_db_status( d->get_db_status() );
                break;
            case 4 :
                m_dirty[4] = 1;
                set_db_name( d->get_db_name() );
                break;
            case 5 :
                m_dirty[5] = 1;
                set_db_zone( d->get_db_zone() );
                break;
            case 6 :
                m_dirty[6] = 1;
                set_db_createtime( d->get_db_createtime() );
                break;
            case 7 :
                m_dirty[7] = 1;
                set_db_level( d->get_db_level() );
                break;
            case 8 :
                m_dirty[8] = 1;
                set_db_viplevel( d->get_db_viplevel() );
                break;
            case 9 :
                m_dirty[9] = 1;
                set_db_avatar( d->get_db_avatar() );
                break;
            case 10 :
                m_dirty[10] = 1;
                set_db_battlepoint( d->get_db_battlepoint() );
                break;
            case 11 :
                m_dirty[11] = 1;
                set_db_lastercity( d->get_db_lastercity() );
                break;
            case 12 :
                m_dirty[12] = 1;
                set_db_lastlogintime( d->get_db_lastlogintime() );
                break;
            case 13 :
                m_dirty[13] = 1;
                set_db_lastlogouttime( d->get_db_lastlogouttime() );
                break;
            case 14 :
                m_dirty[14] = 1;
                set_db_bannedtime( d->get_db_bannedtime() );
                break;
            case 15 :
                m_dirty[15] = 1;
                set_db_bannedreason( d->get_db_bannedreason() );
                break;
            case 16 :
                m_dirty[16] = 1;
                set_db_shutuptime( d->get_db_shutuptime() );
                break;
            case 17 :
                m_dirty[17] = 1;
                set_db_opensystems( d->get_db_opensystems() );
                break;
            case 18 :
                m_dirty[18] = 1;
                set_db_mainprogress( d->get_db_mainprogress() );
                break;
            case 19 :
                m_dirty[19] = 1;
                set_db_friendscount( d->get_db_friendscount() );
                break;
            case 20 :
                m_dirty[20] = 1;
                set_db_settings( d->get_db_settings() );
                break;
            case 21 :
                m_dirty[21] = 1;
                set_db_arenastatus( d->get_db_arenastatus() );
                break;
        }
    }
}

bool CharBrief_Data::match( const ISQLData * data ) const
{
    assert( data->tablename() == TABLENAME );
    const CharBrief_Data * d = static_cast<const CharBrief_Data *>(data);

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
                if ( get_db_account() != d->get_db_account() ) return false;
                break;
            case 2 :
                if ( get_db_storeaccount() != d->get_db_storeaccount() ) return false;
                break;
            case 3 :
                if ( get_db_status() != d->get_db_status() ) return false;
                break;
            case 4 :
                if ( get_db_name() != d->get_db_name() ) return false;
                break;
            case 5 :
                if ( get_db_zone() != d->get_db_zone() ) return false;
                break;
            case 6 :
                if ( get_db_createtime() != d->get_db_createtime() ) return false;
                break;
            case 7 :
                if ( get_db_level() != d->get_db_level() ) return false;
                break;
            case 8 :
                if ( get_db_viplevel() != d->get_db_viplevel() ) return false;
                break;
            case 9 :
                if ( get_db_avatar() != d->get_db_avatar() ) return false;
                break;
            case 10 :
                if ( get_db_battlepoint() != d->get_db_battlepoint() ) return false;
                break;
            case 11 :
                if ( get_db_lastercity() != d->get_db_lastercity() ) return false;
                break;
            case 12 :
                if ( get_db_lastlogintime() != d->get_db_lastlogintime() ) return false;
                break;
            case 13 :
                if ( get_db_lastlogouttime() != d->get_db_lastlogouttime() ) return false;
                break;
            case 14 :
                if ( get_db_bannedtime() != d->get_db_bannedtime() ) return false;
                break;
            case 15 :
                if ( get_db_bannedreason() != d->get_db_bannedreason() ) return false;
                break;
            case 16 :
                if ( get_db_shutuptime() != d->get_db_shutuptime() ) return false;
                break;
            case 17 :
                if ( get_db_opensystems() != d->get_db_opensystems() ) return false;
                break;
            case 18 :
                if ( get_db_mainprogress() != d->get_db_mainprogress() ) return false;
                break;
            case 19 :
                if ( get_db_friendscount() != d->get_db_friendscount() ) return false;
                break;
            case 20 :
                if ( get_db_settings() != d->get_db_settings() ) return false;
                break;
            case 21 :
                if ( get_db_arenastatus() != d->get_db_arenastatus() ) return false;
                break;
        }
    }
    return true;
}

bool CharBrief_Data::parse( const Slices & result )
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
                m_db_account = result[i].ToString();
                break;
            case 2 :
                m_db_storeaccount = result[i].ToString();
                break;
            case 3 :
                m_db_status = (int8_t)std::atoi( result[i].ToString().c_str() );
                break;
            case 4 :
                m_db_name = result[i].ToString();
                break;
            case 5 :
                m_db_zone = (uint16_t)std::atoi( result[i].ToString().c_str() );
                break;
            case 6 :
                m_db_createtime = (int64_t)std::atoll( result[i].ToString().c_str() );
                break;
            case 7 :
                m_db_level = (uint32_t)std::atoi( result[i].ToString().c_str() );
                break;
            case 8 :
                m_db_viplevel = (uint32_t)std::atoi( result[i].ToString().c_str() );
                break;
            case 9 :
                m_db_avatar = (uint32_t)std::atoi( result[i].ToString().c_str() );
                break;
            case 10 :
                m_db_battlepoint = (uint64_t)std::atoll( result[i].ToString().c_str() );
                break;
            case 11 :
                m_db_lastercity = result[i].ToString();
                break;
            case 12 :
                m_db_lastlogintime = (int64_t)std::atoll( result[i].ToString().c_str() );
                break;
            case 13 :
                m_db_lastlogouttime = (int64_t)std::atoll( result[i].ToString().c_str() );
                break;
            case 14 :
                m_db_bannedtime = (int64_t)std::atoll( result[i].ToString().c_str() );
                break;
            case 15 :
                m_db_bannedreason = result[i].ToString();
                break;
            case 16 :
                m_db_shutuptime = (int64_t)std::atoll( result[i].ToString().c_str() );
                break;
            case 17 :
                m_db_opensystems = result[i].ToString();
                break;
            case 18 :
                m_db_mainprogress = result[i].ToString();
                break;
            case 19 :
                m_db_friendscount = (uint32_t)std::atoi( result[i].ToString().c_str() );
                break;
            case 20 :
                m_db_settings = (uint64_t)std::atoll( result[i].ToString().c_str() );
                break;
            case 21 :
                m_db_arenastatus = (int8_t)std::atoi( result[i].ToString().c_str() );
                break;
        }
    }

    return true;
}

void CharBrief_Data::registering( kaguya::State * state )
{
    (*state)["CharBrief_Data"].setClass( kaguya::UserdataMetatable<CharBrief_Data, ISQLData>()
        .setConstructors<CharBrief_Data(), CharBrief_Data(uint64_t, const std::string &)>()
        .addFunction( "get_db_roleid", &CharBrief_Data::get_db_roleid )
        .addFunction( "set_db_roleid", &CharBrief_Data::set_db_roleid )
        .addFunction( "get_db_account", &CharBrief_Data::get_db_account )
        .addFunction( "set_db_account", &CharBrief_Data::set_db_account )
        .addFunction( "get_db_storeaccount", &CharBrief_Data::get_db_storeaccount )
        .addFunction( "set_db_storeaccount", &CharBrief_Data::set_db_storeaccount )
        .addFunction( "get_db_status", &CharBrief_Data::get_db_status )
        .addFunction( "set_db_status", &CharBrief_Data::set_db_status )
        .addFunction( "get_db_name", &CharBrief_Data::get_db_name )
        .addFunction( "set_db_name", &CharBrief_Data::set_db_name )
        .addFunction( "get_db_zone", &CharBrief_Data::get_db_zone )
        .addFunction( "set_db_zone", &CharBrief_Data::set_db_zone )
        .addFunction( "get_db_createtime", &CharBrief_Data::get_db_createtime )
        .addFunction( "set_db_createtime", &CharBrief_Data::set_db_createtime )
        .addFunction( "get_db_level", &CharBrief_Data::get_db_level )
        .addFunction( "set_db_level", &CharBrief_Data::set_db_level )
        .addFunction( "get_db_viplevel", &CharBrief_Data::get_db_viplevel )
        .addFunction( "set_db_viplevel", &CharBrief_Data::set_db_viplevel )
        .addFunction( "get_db_avatar", &CharBrief_Data::get_db_avatar )
        .addFunction( "set_db_avatar", &CharBrief_Data::set_db_avatar )
        .addFunction( "get_db_battlepoint", &CharBrief_Data::get_db_battlepoint )
        .addFunction( "set_db_battlepoint", &CharBrief_Data::set_db_battlepoint )
        .addFunction( "get_db_lastercity", &CharBrief_Data::get_db_lastercity )
        .addFunction( "set_db_lastercity", &CharBrief_Data::set_db_lastercity )
        .addFunction( "get_db_lastlogintime", &CharBrief_Data::get_db_lastlogintime )
        .addFunction( "set_db_lastlogintime", &CharBrief_Data::set_db_lastlogintime )
        .addFunction( "get_db_lastlogouttime", &CharBrief_Data::get_db_lastlogouttime )
        .addFunction( "set_db_lastlogouttime", &CharBrief_Data::set_db_lastlogouttime )
        .addFunction( "get_db_bannedtime", &CharBrief_Data::get_db_bannedtime )
        .addFunction( "set_db_bannedtime", &CharBrief_Data::set_db_bannedtime )
        .addFunction( "get_db_bannedreason", &CharBrief_Data::get_db_bannedreason )
        .addFunction( "set_db_bannedreason", &CharBrief_Data::set_db_bannedreason )
        .addFunction( "get_db_shutuptime", &CharBrief_Data::get_db_shutuptime )
        .addFunction( "set_db_shutuptime", &CharBrief_Data::set_db_shutuptime )
        .addFunction( "get_db_opensystems", &CharBrief_Data::get_db_opensystems )
        .addFunction( "set_db_opensystems", &CharBrief_Data::set_db_opensystems )
        .addFunction( "get_db_mainprogress", &CharBrief_Data::get_db_mainprogress )
        .addFunction( "set_db_mainprogress", &CharBrief_Data::set_db_mainprogress )
        .addFunction( "get_db_friendscount", &CharBrief_Data::get_db_friendscount )
        .addFunction( "set_db_friendscount", &CharBrief_Data::set_db_friendscount )
        .addFunction( "get_db_settings", &CharBrief_Data::get_db_settings )
        .addFunction( "set_db_settings", &CharBrief_Data::set_db_settings )
        .addFunction( "get_db_arenastatus", &CharBrief_Data::get_db_arenastatus )
        .addFunction( "set_db_arenastatus", &CharBrief_Data::set_db_arenastatus )
        .addStaticFunction( "convert", [] ( ISQLData * data ) -> CharBrief_Data * { return static_cast<CharBrief_Data *>( data ); } )
    );
}

bool CharBrief_Data::invoke( kaguya::State * state, const std::string & script )
{
    (*state)["CharBrief"] = this;
    state->dostring( script.c_str() );
    (*state)["CharBrief"] = nullptr;
    return true;
}

void CharBrief_Data::copyfrom( const CharBrief_Data & data )
{
    // Field: storeaccount, Index: 2
    m_dirty[2] = 1;
    m_db_storeaccount = data.get_db_storeaccount();
    // Field: status, Index: 3
    m_dirty[3] = 1;
    m_db_status = data.get_db_status();
    // Field: name, Index: 4
    m_dirty[4] = 1;
    m_db_name = data.get_db_name();
    // Field: zone, Index: 5
    m_dirty[5] = 1;
    m_db_zone = data.get_db_zone();
    // Field: createtime, Index: 6
    m_dirty[6] = 1;
    m_db_createtime = data.get_db_createtime();
    // Field: level, Index: 7
    m_dirty[7] = 1;
    m_db_level = data.get_db_level();
    // Field: viplevel, Index: 8
    m_dirty[8] = 1;
    m_db_viplevel = data.get_db_viplevel();
    // Field: avatar, Index: 9
    m_dirty[9] = 1;
    m_db_avatar = data.get_db_avatar();
    // Field: battlepoint, Index: 10
    m_dirty[10] = 1;
    m_db_battlepoint = data.get_db_battlepoint();
    // Field: lastercity, Index: 11
    m_dirty[11] = 1;
    m_db_lastercity = data.get_db_lastercity();
    // Field: lastlogintime, Index: 12
    m_dirty[12] = 1;
    m_db_lastlogintime = data.get_db_lastlogintime();
    // Field: lastlogouttime, Index: 13
    m_dirty[13] = 1;
    m_db_lastlogouttime = data.get_db_lastlogouttime();
    // Field: bannedtime, Index: 14
    m_dirty[14] = 1;
    m_db_bannedtime = data.get_db_bannedtime();
    // Field: bannedreason, Index: 15
    m_dirty[15] = 1;
    m_db_bannedreason = data.get_db_bannedreason();
    // Field: shutuptime, Index: 16
    m_dirty[16] = 1;
    m_db_shutuptime = data.get_db_shutuptime();
    // Field: opensystems, Index: 17
    m_dirty[17] = 1;
    m_db_opensystems = data.get_db_opensystems();
    // Field: mainprogress, Index: 18
    m_dirty[18] = 1;
    m_db_mainprogress = data.get_db_mainprogress();
    // Field: friendscount, Index: 19
    m_dirty[19] = 1;
    m_db_friendscount = data.get_db_friendscount();
    // Field: settings, Index: 20
    m_dirty[20] = 1;
    m_db_settings = data.get_db_settings();
    // Field: arenastatus, Index: 21
    m_dirty[21] = 1;
    m_db_arenastatus = data.get_db_arenastatus();
}

}
