
#include "base/base.h"
#include "utils/random.h"
#include "channelroute.h"

struct RouterMeta
{
    SceneID sceneid;
    HostID hostid;
};

ChannelRouter::ChannelRouter( size_t count )
{
    if ( count > 0 ) {
        char metafile[1024];
        snprintf( metafile, 1023, "%s/copy.rtstat", APP_META_PATH );
        m_RTState.link( metafile, count );

        // 从meta初始化ChannelRoute
        m_RTState.for_each( [&]( const auto & kv ) {
            auto data = m_RTState.at( kv.second );
            if ( data != nullptr ) {
                update( kv.first, data->hostid, 0 );
            }
        } );
    }
}

ChannelRouter::~ChannelRouter()
{
    m_CopyMap.clear();
    m_ChannelMap.clear();
}

HostID ChannelRouter::get( SceneID id ) const
{
    auto it = m_ChannelMap.find( id );
    if ( it != m_ChannelMap.end() ) {
        return it->second.hostid;
    }

    return 0;
}

HostID ChannelRouter::select( const HostIDs & ids ) const
{
    if ( ids.empty() ) {
        LOG_ERROR( "ChannelRouter::select() : hostid list empty .\n" );
        return 0;
    }

    HostIDs hostids;
    for ( auto id : ids ) {
        auto it = m_CopyMap.find( id );
        if ( it == m_CopyMap.end() ) {
            hostids.emplace_back( id );
        }
    }

    auto result = utils::random_thread_local::get( hostids );
    if ( result != hostids.end() ) {
        return *result;
    }

    auto it = std::min_element( m_CopyMap.begin(), m_CopyMap.end(), []( const auto & c1, const auto & c2 ) -> bool {
        return c1.second < c2.second;
    } );
    return it->first;
}

void ChannelRouter::fill( std::vector<ChannelInfo> & tablelist ) const
{
    tablelist.reserve( m_ChannelMap.size() );

    for ( auto & kv : m_ChannelMap ) {
        tablelist.emplace_back(
            kv.second.sceneid, kv.second.hostid, kv.second.count );
    }
}

void ChannelRouter::update( SceneID id, HostID hostid, int32_t count )
{
    if ( hostid == 0 ) {
        assert( hostid != 0 && "hostid is zero" );
        return;
    }

    int32_t change = 0;

    auto pos = m_ChannelMap.find( id );
    if ( pos != m_ChannelMap.end() ) {
        assert( pos->second.hostid == hostid );
        if ( count == -1 ) {
            change = 1;
            ++pos->second.count;
        } else {
            change = count - pos->second.count;
            pos->second.count = count;
        }
    } else {
        change = count < 0 ? 1 : count;
        m_ChannelMap.emplace( id, ChannelInfo( id, hostid, change ) );
    }

    auto result = m_CopyMap.find( hostid );
    if ( result != m_CopyMap.end() ) {
        result->second += change;
    } else {
        m_CopyMap.emplace( hostid, change );
    }

    // 更新meta
    auto slot = m_RTState.search( id );
    if ( slot != -1 ) {
        auto data = m_RTState.at( slot );
        if ( data != nullptr ) {
            data->hostid = hostid;
        }
    }
}

void ChannelRouter::remove( SceneID id )
{
    auto it = m_ChannelMap.find( id );
    if ( it != m_ChannelMap.end() ) {
        auto result = m_CopyMap.find( it->second.hostid );
        if ( result != m_CopyMap.end() ) {
            int32_t count = result->second - it->second.count;
            if ( count > 0 ) {
                result->second = count;
            } else {
                m_CopyMap.erase( result );
            }
        }
        m_ChannelMap.erase( it );
    }

    // 更新meta
    m_RTState.erase( id );
}
