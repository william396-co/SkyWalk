
#include <mutex>
#include <cstdlib>
#include <algorithm>

#include "utils/spinlock.h"
#include "harbor.h"

class HarborLock : public EmptyLock
{
public:
    HarborLock() {}
    virtual ~HarborLock() {}
    virtual void lock() { m_Lock.lock(); }
    virtual void unlock() { m_Lock.unlock(); }

private:
    utils::SpinLock m_Lock;
};

Harbor::Harbor( bool withlock )
    : m_Withlock( withlock )
{
    m_Lock = withlock ? new HarborLock() : new EmptyLock();
    assert( m_Lock != nullptr );
}

Harbor::~Harbor()
{
    for ( const auto & value : m_NodeMap ) {
        delete value.second;
    }

    delete m_Lock;
    m_NodeMap.clear();
    m_SessionMap.clear();
}

HostID Harbor::splice( ZoneID id, HostID hostid )
{
    assert( id <= ZONEID_MAX
        && hostid <= ( 1 << 8 ) );
    return ( ( id << 16 ) | hostid );
}

std::pair<ZoneID, HostID> Harbor::split( HostID id )
{
    return std::pair<ZoneID, HostID>( id >> 16, id & 0x0000ffff );
}

bool Harbor::add( Harbor::NodeType ntype, HostType type, HostID id, sid_t sid, const Endpoint & ep )
{
    std::lock_guard<EmptyLock> guard( *m_Lock );

    auto result = m_NodeMap.find( id );
    if ( result != m_NodeMap.end() ) {
        sid_t oldsid = result->second->sid;
        //
        result->second->id = id;
        result->second->sid = sid;
        result->second->ntype = ntype;
        result->second->endpoint = ep;
        //
        m_SessionMap.erase( oldsid );
        m_SessionMap.emplace( sid, id );
    } else {
        m_SessionMap.emplace( sid, id );
        m_NodeMap.emplace( id, new Node( ntype, type, id, sid, ep ) );
    }

    return true;
}

void Harbor::del( sid_t sid )
{
    std::lock_guard<EmptyLock> guard( *m_Lock );

    auto result = m_SessionMap.find( sid );
    if ( result == m_SessionMap.end() ) {
        return;
    }

    auto pos = m_NodeMap.find( result->second );
    if ( pos != m_NodeMap.end() ) {
        delete pos->second;
        m_NodeMap.erase( pos );
    }

    m_SessionMap.erase( result );
}

HostID Harbor::hostid( sid_t sid ) const
{
    std::lock_guard<EmptyLock> guard( *m_Lock );

    auto result = m_SessionMap.find( sid );
    if ( result != m_SessionMap.end() ) {
        return result->second;
    }

    return 0;
}

HostType Harbor::hosttype( sid_t sid ) const
{
    std::lock_guard<EmptyLock> guard( *m_Lock );

    auto result = m_SessionMap.find( sid );
    if ( result != m_SessionMap.end() ) {
        auto pos = m_NodeMap.find( result->second );
        if ( pos != m_NodeMap.end() ) {
            return pos->second->type;
        }
    }

    return HostType::None;
}

sid_t Harbor::sid( HostID id ) const
{
    std::lock_guard<EmptyLock> guard( *m_Lock );

    auto result = m_NodeMap.find( id );
    if ( result != m_NodeMap.end() ) {
        return result->second->sid;
    }

    return 0;
}

const Endpoint * Harbor::endpoint( HostID id ) const
{
    assert( m_Withlock == false );
    std::lock_guard<EmptyLock> guard( *m_Lock );

    auto result = m_NodeMap.find( id );
    if ( result != m_NodeMap.end() ) {
        return &( result->second->endpoint );
    }

    return nullptr;
}

void Harbor::sids( sids_t & sids ) const
{
    std::lock_guard<EmptyLock> guard( *m_Lock );
    std::for_each( m_NodeMap.begin(),
        m_NodeMap.end(), [&]( auto & node ) { sids.push_back(node.second->sid); } );
}

void Harbor::sids( HostType type, sids_t & sids ) const
{
    std::lock_guard<EmptyLock> guard( *m_Lock );
    std::for_each( m_NodeMap.begin(),
        m_NodeMap.end(), [&]( auto & node ) {
            if ( node.second->type == type ) {
                sids.push_back( node.second->sid );
            }
        } );
}

void Harbor::sids( const HostTypes & types, sids_t & sids ) const
{
    if ( types.empty() ) {
        return;
    } else if ( types.size() == 1 ) {
        return this->sids( types.front(), sids );
    }

    std::lock_guard<EmptyLock> guard( *m_Lock );

    std::for_each( m_NodeMap.begin(),
        m_NodeMap.end(), [&]( auto & node ) {
            if ( types.end()
                != std::find( types.begin(), types.end(), node.second->type ) ) {
                sids.push_back( node.second->sid );
            }
        } );
}

bool Harbor::has( HostID id, HostType type ) const
{
    std::lock_guard<EmptyLock> guard( *m_Lock );

    auto result = m_NodeMap.find( id );
    if ( result != m_NodeMap.end() ) {
        return result->second->type == type;
    }

    return false;
}

bool Harbor::merge( const HostIDs & settings, sids_t & shutdowns, HostIDs & connects )
{
    std::lock_guard<EmptyLock> guard( *m_Lock );

    // 移除当前列表中非法的
    for ( auto it = m_NodeMap.begin(); it != m_NodeMap.end(); ) {
        if ( it->second->ntype == Harbor::ACCEPT ) {
            ++it;
        } else if ( settings.end()
            != std::find( settings.begin(), settings.end(), it->first ) ) {
            ++it;
        } else {
            m_SessionMap.erase( it->second->sid );
            shutdowns.push_back( it->second->sid );
            delete it->second;
            it = m_NodeMap.erase( it );
        }
    }

    // 添加新的服务器
    for ( auto hostid : settings ) {
        if ( m_NodeMap.find( hostid ) == m_NodeMap.end() ) {
            connects.push_back( hostid );
        }
    }

    return true;
}

uint32_t Harbor::random( HostType type ) const
{
    std::vector<HostID> nodes;
    std::lock_guard<EmptyLock> guard( *m_Lock );

    std::for_each( m_NodeMap.begin(),
        m_NodeMap.end(), [&]( auto & node ) {
            if ( node.second->type == type ) {
                nodes.push_back( node.first );
            }
        } );

    if ( nodes.empty() ) {
        return 0;
    }

    return nodes[rand() % nodes.size()];
}

bool Harbor::random( HostType type, sid_t & id, Endpoint & ep ) const
{
    std::lock_guard<EmptyLock> guard( *m_Lock );

    HostID hid = random( type );
    if ( hid == 0 ) {
        return false;
    }

    id = sid( hid );
    ep = *( endpoint( hid ) );
    return true;
}
