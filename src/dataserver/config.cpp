
#include <cassert>
#include <cstring>
#include <algorithm>

#include "base/base.h"
#include "utils/file.h"
#include "utils/utility.h"

#include "config.h"

namespace data {

AppConfigfile::AppConfigfile()
    : ISysConfigfile( HostType::Data, "config/dataserver.conf" ),
      m_DBThreadCount( 0 ),
      m_FlushInterval( 0 ),
      m_CacheLifetime( 0 ),
      m_StartupScript( "startup.py" ),
      m_RoutineScript( "scripts/routine.lua" ),
      m_DBPort( 0 )
{}

AppConfigfile::~AppConfigfile()
{}

bool AppConfigfile::load()
{
    bool rc = false;
    utils::ConfigFile doc( m_Configfile.c_str() );

    rc = doc.open();
    assert( rc && "AppConfigfile::load() failed" );

    // Global
    doc.get( "Global", "loglevel", m_LogLevel );
    doc.get( "Global", "logfilesize", m_LogFilesize );
    doc.get( "Global", "dbthreads", m_DBThreadCount );
    doc.get( "Global", "flushinterval", m_FlushInterval );
    doc.get( "Global", "cachelifetime", m_CacheLifetime );
    doc.get( "Global", "masterserver", m_MasterEndpoint );
    doc.get( "Global", "startupscript", m_StartupScript );
    doc.get( "Global", "routinescript", m_RoutineScript );
    doc.get( "Global", "ignoreerrors", m_IgnoreErrors );
    if ( m_IgnoreErrors.empty() ) {
        //
        // MySQL Error Codes and Messages
        // https://dev.mysql.com/doc/refman/5.7/en/error-handling.html
        //
        // > 2000   - Client Error Codes
        // < 2000   - Server Error Codes
        //
        // 忽略的错误列表
        // 1054     - ER_BAD_FIELD_ERROR
        // 1062     - ER_DUP_ENTRY
        // 1064     - ER_PARSE_ERROR
        // 1065     - ER_EMPTY_QUERY
        //

        utils::Utility::append( m_IgnoreErrors, 1054 );
        utils::Utility::append( m_IgnoreErrors, 1062 );
        utils::Utility::append( m_IgnoreErrors, 1064 );
        utils::Utility::append( m_IgnoreErrors, 1065 );
    }

    // Database
    doc.get( "Database", "host", m_DBHost );
    doc.get( "Database", "port", m_DBPort );
    doc.get( "Database", "username", m_DBUsername );
    doc.get( "Database", "password", m_DBPassword );
    doc.get( "Database", "charsets", m_DBCharsets );
    doc.get( "Database", "database", m_Database );

    doc.close();
    LOG_TRACE( "AppConfigfile::load('{}') succeed .\n", m_Configfile );

    return rc;
}

bool AppConfigfile::reload()
{
    bool rc = false;

    utils::ConfigFile doc( m_Configfile.c_str() );
    rc = doc.open();
    assert( rc && "AppConfigfile::reload() failed" );

    // Global
    doc.get( "Global", "loglevel", m_LogLevel );
    doc.get( "Global", "logfilesize", m_LogFilesize );
    doc.get( "Global", "flushinterval", m_FlushInterval );
    doc.get( "Global", "cachelifetime", m_CacheLifetime );
    doc.close();

    LOG_TRACE( "AppConfigfile::reload('{}') succeed .\n", m_Configfile );

    return rc;
}

void AppConfigfile::unload()
{
    cleanup();

    m_DBThreadCount = 0;
    m_FlushInterval = 0;
    m_CacheLifetime = 0;

    m_DBHost.clear();
    m_DBPort = 0;
    m_DBUsername.clear();
    m_DBPassword.clear();
    m_DBCharsets.clear();
    m_Database.clear();
}

std::string AppConfigfile::getRootPath() const
{
    std::string routine_file, root_path;
    utils::PathUtils( g_AppConfig.getRoutineScript() ).split( root_path, routine_file );
    return root_path;
}

bool AppConfigfile::isIgnoreErrorcode( int32_t code ) const
{
    return m_IgnoreErrors.end()
        != std::find( m_IgnoreErrors.begin(), m_IgnoreErrors.end(), code );
}

} // namespace data
