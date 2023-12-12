
#include <cstdio>
#include <cassert>
#include <cinttypes>
#include <cstdlib>

#include "lua/kaguya.hpp"
#include "utils/integer.h"
#include "utils/streambuf.h"

#include "Mail.hpp"

namespace data
{

// Tablename
const std::string Mail_Data::TABLENAME = "Mail";

// VARCHAR Length Limit
const size_t Mail_Data::MAX_DB_SENDERNAME_LENGTH = 255;
const size_t Mail_Data::MAX_DB_TITLE_LENGTH = 255;
const size_t Mail_Data::MAX_DB_CONTENT_LENGTH = 1024;
const size_t Mail_Data::MAX_DB_PARAMLIST_LENGTH = 1024;
const size_t Mail_Data::MAX_DB_ATTACHMENT_LENGTH = 10240;

Mail_Data::Mail_Data()
{
    clean();
    // Fields
    m_db_mailid = 0;
    m_db_type = 0;
    m_db_sendtime = 0;
    m_db_templateid = 0;
    m_db_status = 0;
    m_db_sender = 0;
    m_db_sendername = "";
    m_db_roleid = 0;
    m_db_tag = 0;
    m_db_keepdays = 0;
    m_db_title = "";
    m_db_content = "";
    m_db_paramlist = "";
    m_db_attachment = "";
}

Mail_Data::Mail_Data( uint64_t mailid, uint64_t roleid )
{
    clean();
    // Fields
    m_db_mailid = mailid;
    m_db_type = 0;
    m_db_sendtime = 0;
    m_db_templateid = 0;
    m_db_status = 0;
    m_db_sender = 0;
    m_db_sendername = "";
    m_db_roleid = roleid;
    m_db_tag = 0;
    m_db_keepdays = 0;
    m_db_title = "";
    m_db_content = "";
    m_db_paramlist = "";
    m_db_attachment = "";
}

Mail_Data::~Mail_Data()
{
    m_db_mailid = 0;
    m_db_type = 0;
    m_db_sendtime = 0;
    m_db_templateid = 0;
    m_db_status = 0;
    m_db_sender = 0;
    m_db_sendername.clear();
    m_db_roleid = 0;
    m_db_tag = 0;
    m_db_keepdays = 0;
    m_db_title.clear();
    m_db_content.clear();
    m_db_paramlist.clear();
    m_db_attachment.clear();
}

void Mail_Data::clean()
{
    m_method = DBMethod::None;
    bzero( &m_dirty, sizeof(m_dirty) );
}

ISQLData * Mail_Data::clone( bool isfull ) const
{
    Mail_Data * data = new Mail_Data();

    if ( data != nullptr && isfull )
    {
        data->m_method = m_method;
        memcpy( data->m_dirty, m_dirty, NFIELDS );

        data->m_db_mailid = m_db_mailid;
        data->m_db_type = m_db_type;
        data->m_db_sendtime = m_db_sendtime;
        data->m_db_templateid = m_db_templateid;
        data->m_db_status = m_db_status;
        data->m_db_sender = m_db_sender;
        data->m_db_sendername = m_db_sendername;
        data->m_db_roleid = m_db_roleid;
        data->m_db_tag = m_db_tag;
        data->m_db_keepdays = m_db_keepdays;
        data->m_db_title = m_db_title;
        data->m_db_content = m_db_content;
        data->m_db_paramlist = m_db_paramlist;
        data->m_db_attachment = m_db_attachment;
    }

    return data;
}

bool Mail_Data::isDirty() const
{
    for ( uint16_t i = 0; i < NFIELDS; ++i )
    {
        if ( m_dirty[i] != 0 ) return true;
    }

    return false;
}

bool Mail_Data::isKeydirty() const
{
    return m_dirty[0] == 1 || m_dirty[7] == 1;
}

void Mail_Data::keystring( std::string & k ) const
{
    k = utils::Integer::toString( m_db_mailid );
}

void Mail_Data::indexstring( uint8_t op, std::string & k ) const
{
    if ( likely(op == eCodec_All) )
    {
        k = utils::Integer::toString( m_db_roleid );
    }
    else
    {
        if ( m_dirty[7] == 1 )
        {
            if ( !k.empty() ) k += "#";
            k += utils::Integer::toString( m_db_roleid );
        }
    }
}

void Mail_Data::autoincrease( uint8_t source, const Slice & value )
{
}

bool Mail_Data::query( std::string & sqlcmd, std::vector<std::string> & escapevalues, bool all )
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
            if ( !fields.empty() && ( i == 7 ) )
            {
                fields += " AND ";
            }

            switch ( i )
            {
                case 7 :
                    fields += "`roleid`=";
                    fields += utils::Integer::toString( m_db_roleid );
                    break;
            }
        }
        else
        {
            if ( !fields.empty() && ( i == 0 || i == 7 ) )
            {
                fields += " AND ";
            }

            switch ( i )
            {
                case 0 :
                    fields += "`mailid`=";
                    fields += utils::Integer::toString( m_db_mailid );
                    break;
                case 7 :
                    fields += "`roleid`=";
                    fields += utils::Integer::toString( m_db_roleid );
                    break;
            }
        }
    }

    if ( fields.empty() )
    {
        sqlcmd = "SELECT `mailid`,`type`,`sendtime`,`templateid`,`status`,`sender`,`sendername`,`roleid`,`tag`,`keepdays`,`title`,`content`,`paramlist`,`attachment` FROM `Mail`";
    }
    else
    {
        uint32_t len = 32 + 139 + 0 + 4 + fields.size();
        sqlcmd.resize( len );
        len = std::snprintf(
            (char *)sqlcmd.data(), len-1,
            "SELECT `mailid`,`type`,`sendtime`,`templateid`,`status`,`sender`,`sendername`,`roleid`,`tag`,`keepdays`,`title`,`content`,`paramlist`,`attachment` FROM `Mail` WHERE %s", fields.c_str() );
        sqlcmd.resize( len );
    }

    return true;
}

bool Mail_Data::insert( std::string & sqlcmd, std::vector<std::string> & escapevalues )
{
    std::string fields, values;

    for ( uint16_t i = 0; i < NFIELDS; ++i )
    {
        if ( m_dirty[i] == 0 && i != 0 && i != 7 )
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
                fields += "`mailid`";
                values += utils::Integer::toString( m_db_mailid );
                break;
            case 1 :
                fields += "`type`";
                values += utils::Integer::toString( m_db_type );
                break;
            case 2 :
                fields += "`sendtime`";
                values += utils::Integer::toString( m_db_sendtime );
                break;
            case 3 :
                fields += "`templateid`";
                values += utils::Integer::toString( m_db_templateid );
                break;
            case 4 :
                fields += "`status`";
                values += utils::Integer::toString( m_db_status );
                break;
            case 5 :
                fields += "`sender`";
                values += utils::Integer::toString( m_db_sender );
                break;
            case 6 :
                fields += "`sendername`";
                values += "'?'";
                escapevalues.push_back( m_db_sendername );
                break;
            case 7 :
                fields += "`roleid`";
                values += utils::Integer::toString( m_db_roleid );
                break;
            case 8 :
                fields += "`tag`";
                values += utils::Integer::toString( m_db_tag );
                break;
            case 9 :
                fields += "`keepdays`";
                values += utils::Integer::toString( m_db_keepdays );
                break;
            case 10 :
                fields += "`title`";
                values += "'?'";
                escapevalues.push_back( m_db_title );
                break;
            case 11 :
                fields += "`content`";
                values += "'?'";
                escapevalues.push_back( m_db_content );
                break;
            case 12 :
                fields += "`paramlist`";
                values += "'?'";
                escapevalues.push_back( m_db_paramlist );
                break;
            case 13 :
                fields += "`attachment`";
                values += "'?'";
                escapevalues.push_back( m_db_attachment );
                break;
        }
    }

    uint32_t len = 32 + 4 + fields.size() + values.size();

    sqlcmd.resize( len );
    len = std::snprintf( (char *)sqlcmd.data(), len-1,
        "INSERT INTO `Mail` (%s) VALUES (%s)", fields.c_str(), values.c_str() );
    sqlcmd.resize(len);

    return true;
}

bool Mail_Data::update( std::string & sqlcmd, std::vector<std::string> & escapevalues )
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
                dirty += "`mailid`=";
                dirty += utils::Integer::toString( m_db_mailid );
                break;
            case 1 :
                dirty += "`type`=";
                dirty += utils::Integer::toString( m_db_type );
                break;
            case 2 :
                dirty += "`sendtime`=";
                dirty += utils::Integer::toString( m_db_sendtime );
                break;
            case 3 :
                dirty += "`templateid`=";
                dirty += utils::Integer::toString( m_db_templateid );
                break;
            case 4 :
                dirty += "`status`=";
                dirty += utils::Integer::toString( m_db_status );
                break;
            case 5 :
                dirty += "`sender`=";
                dirty += utils::Integer::toString( m_db_sender );
                break;
            case 6 :
                dirty += "`sendername`=";
                dirty += "'?'";
                escapevalues.push_back( m_db_sendername );
                break;
            case 7 :
                dirty += "`roleid`=";
                dirty += utils::Integer::toString( m_db_roleid );
                break;
            case 8 :
                dirty += "`tag`=";
                dirty += utils::Integer::toString( m_db_tag );
                break;
            case 9 :
                dirty += "`keepdays`=";
                dirty += utils::Integer::toString( m_db_keepdays );
                break;
            case 10 :
                dirty += "`title`=";
                dirty += "'?'";
                escapevalues.push_back( m_db_title );
                break;
            case 11 :
                dirty += "`content`=";
                dirty += "'?'";
                escapevalues.push_back( m_db_content );
                break;
            case 12 :
                dirty += "`paramlist`=";
                dirty += "'?'";
                escapevalues.push_back( m_db_paramlist );
                break;
            case 13 :
                dirty += "`attachment`=";
                dirty += "'?'";
                escapevalues.push_back( m_db_attachment );
                break;
        }
    }

    int32_t len = 32 + 4 + dirty.size() + 20 + 32;

    sqlcmd.resize( len );
    len = std::snprintf( (char *)sqlcmd.data(), len-1,
        "UPDATE `Mail` SET %s WHERE `mailid`=%" PRIu64 "", dirty.c_str(), m_db_mailid );
    sqlcmd.resize( len );

    return true;
}

bool Mail_Data::remove( std::string & sqlcmd, std::vector<std::string> & escapevalues )
{
    uint32_t len = 32 + 4 + 20 + 32;

    sqlcmd.resize( len );
    len = std::snprintf( (char *)sqlcmd.data(), len-1,
        "DELETE FROM `Mail` WHERE `mailid`=%" PRIu64 "", m_db_mailid );
    sqlcmd.resize( len );

    return true;
}

bool Mail_Data::replace( std::string & sqlcmd, std::vector<std::string> & escapevalues )
{
    std::string fields, values, dirty;

    for ( uint16_t i = 0; i < NFIELDS; ++i )
    {
        if ( m_dirty[i] == 0 && i != 0 && i != 7 )
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
                fields += "`mailid`";
                values += utils::Integer::toString( m_db_mailid );
                break;
            case 1 :
                fields += "`type`";
                values += utils::Integer::toString( m_db_type );
                break;
            case 2 :
                fields += "`sendtime`";
                values += utils::Integer::toString( m_db_sendtime );
                break;
            case 3 :
                fields += "`templateid`";
                values += utils::Integer::toString( m_db_templateid );
                break;
            case 4 :
                fields += "`status`";
                values += utils::Integer::toString( m_db_status );
                break;
            case 5 :
                fields += "`sender`";
                values += utils::Integer::toString( m_db_sender );
                break;
            case 6 :
                fields += "`sendername`";
                values += "'?'";
                escapevalues.push_back( m_db_sendername );
                break;
            case 7 :
                fields += "`roleid`";
                values += utils::Integer::toString( m_db_roleid );
                break;
            case 8 :
                fields += "`tag`";
                values += utils::Integer::toString( m_db_tag );
                break;
            case 9 :
                fields += "`keepdays`";
                values += utils::Integer::toString( m_db_keepdays );
                break;
            case 10 :
                fields += "`title`";
                values += "'?'";
                escapevalues.push_back( m_db_title );
                break;
            case 11 :
                fields += "`content`";
                values += "'?'";
                escapevalues.push_back( m_db_content );
                break;
            case 12 :
                fields += "`paramlist`";
                values += "'?'";
                escapevalues.push_back( m_db_paramlist );
                break;
            case 13 :
                fields += "`attachment`";
                values += "'?'";
                escapevalues.push_back( m_db_attachment );
                break;
        }
    }
    for ( uint16_t i = 0; i < NFIELDS; ++i )
    {
        if ( m_dirty[i] == 1 && i != 0 && i != 7 )
        {
            if ( !dirty.empty() )
            {
                dirty += ", ";
            }

            switch ( i )
            {
                case 1 :
                    dirty += "`type`=";
                    dirty += utils::Integer::toString( m_db_type );
                    break;
                case 2 :
                    dirty += "`sendtime`=";
                    dirty += utils::Integer::toString( m_db_sendtime );
                    break;
                case 3 :
                    dirty += "`templateid`=";
                    dirty += utils::Integer::toString( m_db_templateid );
                    break;
                case 4 :
                    dirty += "`status`=";
                    dirty += utils::Integer::toString( m_db_status );
                    break;
                case 5 :
                    dirty += "`sender`=";
                    dirty += utils::Integer::toString( m_db_sender );
                    break;
                case 6 :
                    dirty += "`sendername`=";
                    dirty += "'?'";
                    escapevalues.push_back( m_db_sendername );
                    break;
                case 8 :
                    dirty += "`tag`=";
                    dirty += utils::Integer::toString( m_db_tag );
                    break;
                case 9 :
                    dirty += "`keepdays`=";
                    dirty += utils::Integer::toString( m_db_keepdays );
                    break;
                case 10 :
                    dirty += "`title`=";
                    dirty += "'?'";
                    escapevalues.push_back( m_db_title );
                    break;
                case 11 :
                    dirty += "`content`=";
                    dirty += "'?'";
                    escapevalues.push_back( m_db_content );
                    break;
                case 12 :
                    dirty += "`paramlist`=";
                    dirty += "'?'";
                    escapevalues.push_back( m_db_paramlist );
                    break;
                case 13 :
                    dirty += "`attachment`=";
                    dirty += "'?'";
                    escapevalues.push_back( m_db_attachment );
                    break;
            }
        }
    }

    uint32_t len = 64 + 4 + fields.size() + values.size() + dirty.size();

    sqlcmd.resize( len );
    len = std::snprintf( (char *)sqlcmd.data(), len-1,
        "INSERT INTO `Mail` (%s) VALUES (%s) ON DUPLICATE KEY UPDATE %s", fields.c_str(), values.c_str(), dirty.c_str() );
    sqlcmd.resize(len);

    return true;
}

Slice Mail_Data::encode( uint8_t op )
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
            pack.encode( m_db_mailid );
        }
        if ( m_dirty[7] == 1 )
        {
            ++nfields;
            pack.encode( (uint16_t)7 );
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
                    pack.encode( m_db_mailid );
                    break;
                case 1 :
                    ++nfields;
                    pack.encode( i );
                    pack.encode( m_db_type );
                    break;
                case 2 :
                    ++nfields;
                    pack.encode( i );
                    pack.encode( m_db_sendtime );
                    break;
                case 3 :
                    ++nfields;
                    pack.encode( i );
                    pack.encode( m_db_templateid );
                    break;
                case 4 :
                    ++nfields;
                    pack.encode( i );
                    pack.encode( m_db_status );
                    break;
                case 5 :
                    ++nfields;
                    pack.encode( i );
                    pack.encode( m_db_sender );
                    break;
                case 6 :
                    ++nfields;
                    pack.encode( i );
                    pack.encode( m_db_sendername );
                    break;
                case 7 :
                    ++nfields;
                    pack.encode( i );
                    pack.encode( m_db_roleid );
                    break;
                case 8 :
                    ++nfields;
                    pack.encode( i );
                    pack.encode( m_db_tag );
                    break;
                case 9 :
                    ++nfields;
                    pack.encode( i );
                    pack.encode( m_db_keepdays );
                    break;
                case 10 :
                    ++nfields;
                    pack.encode( i );
                    pack.encode( m_db_title );
                    break;
                case 11 :
                    ++nfields;
                    pack.encode( i );
                    pack.encode( m_db_content );
                    break;
                case 12 :
                    ++nfields;
                    pack.encode( i );
                    pack.encode( m_db_paramlist );
                    break;
                case 13 :
                    ++nfields;
                    pack.encode( i );
                    pack.encode( m_db_attachment );
                    break;
            }
        }
    }

    // key fields
    if ( op != eCodec_Query )
    {
        pack.encode( m_db_mailid );
        pack.encode( m_db_roleid );
    }

    // fields number
    pack.reset();
    pack.encode( nfields );

    return pack.slice();
}

bool Mail_Data::decode( const Slice & result, uint8_t op )
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
                unpack.decode( m_db_mailid );
                break;
            case 1 :
                m_dirty[1] = 1;
                unpack.decode( m_db_type );
                break;
            case 2 :
                m_dirty[2] = 1;
                unpack.decode( m_db_sendtime );
                break;
            case 3 :
                m_dirty[3] = 1;
                unpack.decode( m_db_templateid );
                break;
            case 4 :
                m_dirty[4] = 1;
                unpack.decode( m_db_status );
                break;
            case 5 :
                m_dirty[5] = 1;
                unpack.decode( m_db_sender );
                break;
            case 6 :
                m_dirty[6] = 1;
                unpack.decode( m_db_sendername );
                break;
            case 7 :
                m_dirty[7] = 1;
                unpack.decode( m_db_roleid );
                break;
            case 8 :
                m_dirty[8] = 1;
                unpack.decode( m_db_tag );
                break;
            case 9 :
                m_dirty[9] = 1;
                unpack.decode( m_db_keepdays );
                break;
            case 10 :
                m_dirty[10] = 1;
                unpack.decode( m_db_title );
                break;
            case 11 :
                m_dirty[11] = 1;
                unpack.decode( m_db_content );
                break;
            case 12 :
                m_dirty[12] = 1;
                unpack.decode( m_db_paramlist );
                break;
            case 13 :
                m_dirty[13] = 1;
                unpack.decode( m_db_attachment );
                break;
        }
    }

    // keys
    if ( op != eCodec_Query )
    {
        unpack.decode( m_db_mailid );
        unpack.decode( m_db_roleid );
    }

    return true;
}

void Mail_Data::merge( const ISQLData * data )
{
    assert( data->tablename() == TABLENAME );
    const Mail_Data * d = static_cast<const Mail_Data *>(data);

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
                set_db_mailid( d->get_db_mailid() );
                break;
            case 1 :
                m_dirty[1] = 1;
                set_db_type( d->get_db_type() );
                break;
            case 2 :
                m_dirty[2] = 1;
                set_db_sendtime( d->get_db_sendtime() );
                break;
            case 3 :
                m_dirty[3] = 1;
                set_db_templateid( d->get_db_templateid() );
                break;
            case 4 :
                m_dirty[4] = 1;
                set_db_status( d->get_db_status() );
                break;
            case 5 :
                m_dirty[5] = 1;
                set_db_sender( d->get_db_sender() );
                break;
            case 6 :
                m_dirty[6] = 1;
                set_db_sendername( d->get_db_sendername() );
                break;
            case 7 :
                m_dirty[7] = 1;
                set_db_roleid( d->get_db_roleid() );
                break;
            case 8 :
                m_dirty[8] = 1;
                set_db_tag( d->get_db_tag() );
                break;
            case 9 :
                m_dirty[9] = 1;
                set_db_keepdays( d->get_db_keepdays() );
                break;
            case 10 :
                m_dirty[10] = 1;
                set_db_title( d->get_db_title() );
                break;
            case 11 :
                m_dirty[11] = 1;
                set_db_content( d->get_db_content() );
                break;
            case 12 :
                m_dirty[12] = 1;
                set_db_paramlist( d->get_db_paramlist() );
                break;
            case 13 :
                m_dirty[13] = 1;
                set_db_attachment( d->get_db_attachment() );
                break;
        }
    }
}

bool Mail_Data::match( const ISQLData * data ) const
{
    assert( data->tablename() == TABLENAME );
    const Mail_Data * d = static_cast<const Mail_Data *>(data);

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
                if ( get_db_mailid() != d->get_db_mailid() ) return false;
                break;
            case 1 :
                if ( get_db_type() != d->get_db_type() ) return false;
                break;
            case 2 :
                if ( get_db_sendtime() != d->get_db_sendtime() ) return false;
                break;
            case 3 :
                if ( get_db_templateid() != d->get_db_templateid() ) return false;
                break;
            case 4 :
                if ( get_db_status() != d->get_db_status() ) return false;
                break;
            case 5 :
                if ( get_db_sender() != d->get_db_sender() ) return false;
                break;
            case 6 :
                if ( get_db_sendername() != d->get_db_sendername() ) return false;
                break;
            case 7 :
                if ( get_db_roleid() != d->get_db_roleid() ) return false;
                break;
            case 8 :
                if ( get_db_tag() != d->get_db_tag() ) return false;
                break;
            case 9 :
                if ( get_db_keepdays() != d->get_db_keepdays() ) return false;
                break;
            case 10 :
                if ( get_db_title() != d->get_db_title() ) return false;
                break;
            case 11 :
                if ( get_db_content() != d->get_db_content() ) return false;
                break;
            case 12 :
                if ( get_db_paramlist() != d->get_db_paramlist() ) return false;
                break;
            case 13 :
                if ( get_db_attachment() != d->get_db_attachment() ) return false;
                break;
        }
    }
    return true;
}

bool Mail_Data::parse( const Slices & result )
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
                m_db_mailid = (uint64_t)std::atoll( result[i].ToString().c_str() );
                break;
            case 1 :
                m_db_type = (uint8_t)std::atoi( result[i].ToString().c_str() );
                break;
            case 2 :
                m_db_sendtime = (int64_t)std::atoll( result[i].ToString().c_str() );
                break;
            case 3 :
                m_db_templateid = (uint32_t)std::atoi( result[i].ToString().c_str() );
                break;
            case 4 :
                m_db_status = (uint8_t)std::atoi( result[i].ToString().c_str() );
                break;
            case 5 :
                m_db_sender = (uint64_t)std::atoll( result[i].ToString().c_str() );
                break;
            case 6 :
                m_db_sendername = result[i].ToString();
                break;
            case 7 :
                m_db_roleid = (uint64_t)std::atoll( result[i].ToString().c_str() );
                break;
            case 8 :
                m_db_tag = (uint32_t)std::atoi( result[i].ToString().c_str() );
                break;
            case 9 :
                m_db_keepdays = (int32_t)std::atoi( result[i].ToString().c_str() );
                break;
            case 10 :
                m_db_title = result[i].ToString();
                break;
            case 11 :
                m_db_content = result[i].ToString();
                break;
            case 12 :
                m_db_paramlist = result[i].ToString();
                break;
            case 13 :
                m_db_attachment = result[i].ToString();
                break;
        }
    }

    return true;
}

void Mail_Data::registering( kaguya::State * state )
{
    (*state)["Mail_Data"].setClass( kaguya::UserdataMetatable<Mail_Data, ISQLData>()
        .setConstructors<Mail_Data(), Mail_Data(uint64_t, uint64_t)>()
        .addFunction( "get_db_mailid", &Mail_Data::get_db_mailid )
        .addFunction( "set_db_mailid", &Mail_Data::set_db_mailid )
        .addFunction( "get_db_type", &Mail_Data::get_db_type )
        .addFunction( "set_db_type", &Mail_Data::set_db_type )
        .addFunction( "get_db_sendtime", &Mail_Data::get_db_sendtime )
        .addFunction( "set_db_sendtime", &Mail_Data::set_db_sendtime )
        .addFunction( "get_db_templateid", &Mail_Data::get_db_templateid )
        .addFunction( "set_db_templateid", &Mail_Data::set_db_templateid )
        .addFunction( "get_db_status", &Mail_Data::get_db_status )
        .addFunction( "set_db_status", &Mail_Data::set_db_status )
        .addFunction( "get_db_sender", &Mail_Data::get_db_sender )
        .addFunction( "set_db_sender", &Mail_Data::set_db_sender )
        .addFunction( "get_db_sendername", &Mail_Data::get_db_sendername )
        .addFunction( "set_db_sendername", &Mail_Data::set_db_sendername )
        .addFunction( "get_db_roleid", &Mail_Data::get_db_roleid )
        .addFunction( "set_db_roleid", &Mail_Data::set_db_roleid )
        .addFunction( "get_db_tag", &Mail_Data::get_db_tag )
        .addFunction( "set_db_tag", &Mail_Data::set_db_tag )
        .addFunction( "get_db_keepdays", &Mail_Data::get_db_keepdays )
        .addFunction( "set_db_keepdays", &Mail_Data::set_db_keepdays )
        .addFunction( "get_db_title", &Mail_Data::get_db_title )
        .addFunction( "set_db_title", &Mail_Data::set_db_title )
        .addFunction( "get_db_content", &Mail_Data::get_db_content )
        .addFunction( "set_db_content", &Mail_Data::set_db_content )
        .addFunction( "get_db_paramlist", &Mail_Data::get_db_paramlist )
        .addFunction( "set_db_paramlist", &Mail_Data::set_db_paramlist )
        .addFunction( "get_db_attachment", &Mail_Data::get_db_attachment )
        .addFunction( "set_db_attachment", &Mail_Data::set_db_attachment )
        .addStaticFunction( "convert", [] ( ISQLData * data ) -> Mail_Data * { return static_cast<Mail_Data *>( data ); } )
    );
}

bool Mail_Data::invoke( kaguya::State * state, const std::string & script )
{
    (*state)["Mail"] = this;
    state->dostring( script.c_str() );
    (*state)["Mail"] = nullptr;
    return true;
}

void Mail_Data::copyfrom( const Mail_Data & data )
{
    // Field: type, Index: 1
    m_dirty[1] = 1;
    m_db_type = data.get_db_type();
    // Field: sendtime, Index: 2
    m_dirty[2] = 1;
    m_db_sendtime = data.get_db_sendtime();
    // Field: templateid, Index: 3
    m_dirty[3] = 1;
    m_db_templateid = data.get_db_templateid();
    // Field: status, Index: 4
    m_dirty[4] = 1;
    m_db_status = data.get_db_status();
    // Field: sender, Index: 5
    m_dirty[5] = 1;
    m_db_sender = data.get_db_sender();
    // Field: sendername, Index: 6
    m_dirty[6] = 1;
    m_db_sendername = data.get_db_sendername();
    // Field: tag, Index: 8
    m_dirty[8] = 1;
    m_db_tag = data.get_db_tag();
    // Field: keepdays, Index: 9
    m_dirty[9] = 1;
    m_db_keepdays = data.get_db_keepdays();
    // Field: title, Index: 10
    m_dirty[10] = 1;
    m_db_title = data.get_db_title();
    // Field: content, Index: 11
    m_dirty[11] = 1;
    m_db_content = data.get_db_content();
    // Field: paramlist, Index: 12
    m_dirty[12] = 1;
    m_db_paramlist = data.get_db_paramlist();
    // Field: attachment, Index: 13
    m_dirty[13] = 1;
    m_db_attachment = data.get_db_attachment();
}

}
