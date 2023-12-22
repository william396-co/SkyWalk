
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <algorithm>
#include <format>

#include "base/base.h"
#include "base/modules.h"
#include "utils/file.h"
#include "utils/xtime.h"
#include "utils/hashfunc.h"

#include "framework.h"

namespace utils {

template<> bool ConfigFile::get( const char * section, const char * key, ZoneMode & value ) const
{
    const char * _value = getValue( section, key );
    if ( _value != nullptr ) {
        value = (ZoneMode)std::atoi( _value );
        return true;
    }
    return false;
}

template<> bool ConfigFile::get( const char * section, const char * key, Endpoint & value ) const
{
    const char * _value = getValue( section, key );
    if ( _value != nullptr ) {
        char * host = nullptr;
        char * port = const_cast<char *>( _value );
        host = ::strsep( &port, ":" );
        if ( host && port ) {
            value.host = host;
            value.port = std::atoi( port );
            return true;
        }
    }
    return false;
}

} // namespace utils

Framework::Framework()
    : m_ThreadCount( 2 ),
      m_SessTimeout( 30 )
{
    m_HostEndpoints.reserve( 256 );
}

Framework::~Framework()
{
    m_SessTimeout = 0;
    m_ThreadCount = 0;
    m_HostEndpoints.clear();
}

void Framework::clear( bool isglobal )
{
    m_Lock.lock();

    // 只清除从本服务器加载的Framework
    for ( auto it = m_HostEndpoints.begin(); it != m_HostEndpoints.end(); ) {
        if ( !isglobal
            && it->id >= HOSTID_MAX ) {
            ++it;
        } else {
            it = m_HostEndpoints.erase( it );
        }
    }

    m_Lock.unlock();
}

const HostEndpoint * Framework::get( HostID id ) const
{
    for ( const auto & hep : m_HostEndpoints ) {
        if ( hep.id == id ) {
            return &hep;
        }
    }

    return nullptr;
}

bool Framework::has( const Endpoint & endpoint )
{
    std::lock_guard lock( m_Lock );
    const HostEndpoint * ep = get( endpoint );

    return ep != nullptr;
}

bool Framework::get( HostID id, HostEndpoint & endpoint )
{
    std::lock_guard lock( m_Lock );
    const HostEndpoint * ep = get( id );
    if ( ep != nullptr ) endpoint = *ep;

    return ep != nullptr;
}

const HostEndpoint * Framework::get( const Endpoint & endpoint ) const
{
    for ( const auto & hep : m_HostEndpoints ) {
        if ( hep.endpoint == endpoint ) {
            return &hep;
        }
    }

    return nullptr;
}

HostType Framework::getType( HostID id ) const
{
    const HostEndpoint * ep = this->get( id );
    if ( ep != nullptr ) {
        return ep->type;
    }

    return HostType::None;
}

uint32_t Framework::getHostCount( HostType type ) const
{
    uint32_t count = 0;

    for ( const auto & hep : m_HostEndpoints ) {
        if ( hep.type == type ) {
            ++count;
        }
    }

    return count;
}

HostIDs Framework::getHostIDs( HostType type ) const
{
    HostIDs ids;
    getHostIDs( type, ids );
    return ids;
}

void Framework::getHostIDs( HostType type, HostIDs & idlist ) const
{
    for ( const auto & hep : m_HostEndpoints ) {
        if ( hep.type == type
            || type == HostType::Any ) {
            idlist.push_back( hep.id );
        }
    }
}

uint32_t Framework::getIndex( HostID id ) const
{
    HostIDs hosts;
    getHostIDs( getType( id ), hosts );

    for ( size_t i = 0; i < hosts.size(); ++i ) {
        if ( hosts[i] == id ) {
            return i + 1;
        }
    }

    return 0;
}

bool Framework::getEndpoint( HostID id, Endpoint & ep ) const
{
    for ( const auto & hep : m_HostEndpoints ) {
        if ( hep.id == id ) {
            ep = hep.endpoint;
            return true;
        }
    }

    return false;
}

const HostEndpoint * Framework::getEndpoint( HostType type ) const
{
    for ( const auto & hep : m_HostEndpoints ) {
        if ( hep.type == type ) {
            return &hep;
        }
    }

    return nullptr;
}

void Framework::getEndpoints( HostType type, Endpoints & endpoints ) const
{
    for ( const auto & hep : m_HostEndpoints ) {
        if ( hep.type == type ) {
            endpoints.push_back( hep.endpoint );
        }
    }
}

void Framework::getEndpoints( HostType type, HostEndpoints & endpoints ) const
{
    for ( const auto & hep : m_HostEndpoints ) {
        if ( hep.type == type ) {
            endpoints.push_back( hep );
        }
    }
}

void Framework::updateLocalHosts( const HostEndpoints & hosts )
{
    std::lock_guard lock(m_Lock);
    m_HostEndpoints = hosts;
    std::sort( m_HostEndpoints.begin(),
        m_HostEndpoints.end(),
        []( const HostEndpoint & h1, const HostEndpoint & h2 ) {
            return h1.id < h2.id;
        } );
}

void Framework::updateGlobalHosts( const HostEndpoints & hosts )
{
    std::lock_guard lock( m_Lock );
    for ( HostEndpoints::iterator it = m_HostEndpoints.begin(); it != m_HostEndpoints.end(); ) {
        if ( it->id < HOSTID_MAX ) {
            ++it;
        } else {
            it = m_HostEndpoints.erase( it );
        }
    }
    for ( size_t i = 0; i < hosts.size(); ++i ) {
        // 忽略的全局服务器
        if ( m_IgnoreTypeList.end()
            == std::find( m_IgnoreTypeList.begin(), m_IgnoreTypeList.end(), hosts[i].type ) ) {
            m_HostEndpoints.push_back( hosts[i] );
        }
    }

    std::sort( m_HostEndpoints.begin(),
        m_HostEndpoints.end(),
        []( const HostEndpoint & h1, const HostEndpoint & h2 ) {
            return h1.id < h2.id;
        } );
}

std::string Framework::getInformation() const
{
    std::string framework;
    std::string prefix( 43, ' ' );

    for ( const auto & hep : m_HostEndpoints ) {
        framework += std::format( "{}{:#03d} : Type:{}, Endpoint: {} .\n", prefix, hep.id, (uint8_t)hep.type, hep.endpoint );
    }

    return framework;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

bool ISysConfigfile::parse()
{
    assert( m_HostType == HostType::Master || m_HostType == HostType::Global );

    bool rc = false;
    utils::ConfigFile raw_file( m_Configfile.c_str() );

    rc = raw_file.open();
    if ( !rc ) {
        return false;
    }

    // 会话超时时间
    raw_file.get( "Framework", "threadcount", m_Framework.m_ThreadCount );
    raw_file.get( "Framework", "sessiontimeout", m_Framework.m_SessTimeout );

    // host*
    std::vector<std::string> values;
    raw_file.get( "Framework", values );
    for ( const auto & val : values ) {
        std::string value;
        raw_file.get( "Framework", val.c_str(), value );

        if ( val.find( "host" ) == 0 ) {
            uint32_t hostid = std::atoi( val.substr( strlen( "host" ) ).c_str() );

            if ( hostid == 0
                || ( m_HostType == HostType::Master && hostid >= HOSTID_MAX )
                || ( m_HostType == HostType::Global && hostid <= HOSTID_MAX ) ) {
                LOG_ERROR( "Framework::parse() : Framework::{}'s HostID({}) is INVALID .\n", val, hostid );
                continue;
            }

            if ( !value.empty() ) {
                parseEndpoint( hostid, value );
            }
        }
    }
    std::sort( m_Framework.m_HostEndpoints.begin(),
        m_Framework.m_HostEndpoints.end(),
        []( const HostEndpoint & h1, const HostEndpoint & h2 ) {
            return h1.id < h2.id;
        } );

    // Global服务器没有区服配置这个节
    if ( m_HostType == HostType::Master ) {
        // Zone
        std::string opentime;
        std::string zonelist;
        raw_file.get( "Zone", "pf", m_PfID );
        raw_file.get( "Zone", "machine", m_MachineID );
        raw_file.get( "Zone", "appid", m_AppID );
        raw_file.get( "Zone", "zoneindex", m_ZoneID );
        raw_file.get( "Zone", "zonemode", m_ZoneMode );
        raw_file.get( "Zone", "startdate", opentime );
        raw_file.get( "Zone", "mergezonelist", m_MergeZoneList );
        m_StartTime = utils::TimeUtils::getTimestampByDate( opentime.c_str() );
    }

    // 获取Master或者Global服务器ID
    for ( const auto & hep : m_Framework.m_HostEndpoints ) {
        if ( hep.type == m_HostType ) {
            m_ServerID = hep.id;
            break;
        }
    }

    // 关闭配置文件
    raw_file.close();

    return true;
}

void ISysConfigfile::cleanup()
{
    m_Status = 0;
    m_ServerID = 0;
    m_LogLevel = 0;
    m_LogFilesize = 0;
    m_ConnectList.clear();
    m_MasterEndpoint.clear();

    m_PfID = 0;
    m_AppID.clear();
    m_ZoneID = 0;
    m_ZoneMode = ZoneMode::Mix;
    m_StartTime = 0;
    m_MergeZoneList.clear();

    m_Framework.clear( m_HostType == HostType::Global );
}

const Endpoint * ISysConfigfile::getEndpoint() const
{
    const HostEndpoint * ep = m_Framework.get( m_ServerID );
    if ( ep != nullptr ) {
        return &( ep->endpoint );
    }

    return nullptr;
}

bool ISysConfigfile::getExtEndpoint( Endpoint & endpoint )
{
    HostEndpoint ep;
    bool rc = m_Framework.get( m_ServerID, ep );

    endpoint = ep.extendpoint;
    return rc;
}

const Endpoint * ISysConfigfile::getExtEndpoint() const
{
    const HostEndpoint * ep = m_Framework.get( m_ServerID );
    if ( ep != nullptr ) {
        return &( ep->extendpoint );
    }

    return nullptr;
}

void ISysConfigfile::parseEndpoint( HostID id, const std::string & data )
{
    HostEndpoint ep( id );
    int32_t pos = 0, index = 0;

    for ( size_t i = 0; i < data.size(); ++i ) {
        if ( data[i] != ':'
            && i != data.size() - 1 ) {
            continue;
        }

        std::string word = data.substr( pos,
            data[i] == ':' ? i - pos : i - pos + 1 );
        switch ( ++index ) {
            // type
            case 1: ep.type = (HostType)std::atoi( word.c_str() ); break;
            // host
            case 2: ep.endpoint.host = word; break;
            // port
            case 3: ep.endpoint.port = std::atoi( word.c_str() ); break;
            // exthost
            case 4: ep.extendpoint.host = word; break;
            // extport
            case 5: ep.extendpoint.port = std::atoi( word.c_str() ); break;
            // 进程标识
            case 6: ep.procmark = word; break;
            // 连接服务器类型
            case 7: {
                char *w, *brkt;
                const char * sep = ",";
                char * belist = strdup( word.c_str() );
                for ( w = strtok_r( belist, sep, &brkt );
                      w;
                      w = strtok_r( nullptr, sep, &brkt ) ) {
                    ep.connectlist.push_back( (HostType)std::atoi( w ) );
                }
                free( belist );
            } break;
        }

        pos = i + 1;
    }

    m_Framework.m_HostEndpoints.push_back( ep );
}

HostID ISysConfigfile::getJchHostID( HostType type, uint64_t key ) const
{
    HostIDs ids;
    m_Framework.getHostIDs( type, ids );
    if ( ids.empty() ) {
        return 0;
    }
    return ids[utils::HashFunction::jump_consistent_hash( key, ids.size() )];
}

bool ISysConfigfile::isInZone( ZoneID id ) const
{
    return m_ZoneID == id
        || std::find( m_MergeZoneList.begin(), m_MergeZoneList.end(), id ) != m_MergeZoneList.end();
}

int32_t ISysConfigfile::getStartDays( time_t now ) const
{
    return utils::TimeUtils::daysdelta( m_StartTime, now );
}
