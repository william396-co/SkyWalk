
#include <cassert>
#include <cstring>
#include <algorithm>

#include "base/base.h"
#include "utils/file.h"

#include "config.h"

namespace master
{

AppConfigfile::AppConfigfile()
    : ISysConfigfile( HostType::Master, "config/masterserver.conf"),
      m_Cachesize( 0ULL ),
      m_GMTimeoutseconds( 0 )
{
    // 忽略全局服的数据服务器和战斗服务器
    getFramework()->ignore( { HostType::Data, HostType::Copy } );
}

AppConfigfile::~AppConfigfile()
{
    unload();
}

bool AppConfigfile::load()
{
    bool rc = false;
    utils::ConfigFile raw_file( m_Configfile.c_str() );
    rc = raw_file.open();
    assert( rc && "AppConfigfile::load() failed" );

    // Global
    raw_file.get( "Global", "loglevel", m_LogLevel );
    raw_file.get( "Global", "logfilesize", m_LogFilesize );
    raw_file.get( "Global", "logfilesize", m_Cachesize );
    raw_file.get( "Global", "apithreads", m_ApiThreads );
    raw_file.get( "Global", "apiurl", m_ApiURL );
    raw_file.get( "Global", "globalserver", m_GlobalEndpoint );

    // GMService
    raw_file.get( "GMService", "bindhost", m_GMEndpoint.host );
    raw_file.get( "GMService", "listenport", m_GMEndpoint.port );
    raw_file.get( "GMService", "timeoutseconds", m_GMTimeoutseconds );

    // 加载框架配置
    parse();

    LOG_INFO( "AppConfigfile::load('{}', IgnoreList:{}) succeed .\n", m_Configfile, getFramework()->ignore() );
    raw_file.close();

    return rc;
}

bool AppConfigfile::reload()
{
    unload();
    load();

    return true;
}

void AppConfigfile::unload()
{
    m_Cachesize = 0ULL;
    m_ApiURL.clear();
    m_GMEndpoint.clear();
    m_GMTimeoutseconds = 0;
    m_GlobalEndpoint.clear();

    cleanup();
}

}
