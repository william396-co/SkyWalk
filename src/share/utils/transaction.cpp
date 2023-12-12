
#include "transaction.h"

namespace utils {

Transaction::Transaction()
    : m_TransID( 0 ),
      m_Manager( nullptr )
{}

Transaction::~Transaction()
{
    if ( m_TransID != 0 ) {
        m_Manager->remove( m_TransID );
        m_TransID = 0;
    }
}

bool Transaction::onTimer( uint32_t & timeout )
{
    this->onTimeout();
    return false;
}

void Transaction::onEnd()
{
    // 移除事务
    m_Manager->remove( m_TransID );
    m_TransID = 0;
    delete this;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

TransactionManager::TransactionManager( TimeWheel * driver )
    : m_TimeDriver( driver ),
      m_UniqueTransID( 1 )
{}

TransactionManager::~TransactionManager()
{
    for ( const auto & p : m_TransMap ) {
        if ( p.second == nullptr ) {
            continue;
        }

        m_TimeDriver->cancel( p.second );
        p.second->setTransID( 0 );
        p.second->setManager( nullptr );
        delete p.second;
    }
    m_TransMap.clear();
}

TransID TransactionManager::append( Transaction * t, uint32_t seconds )
{
    while ( m_TransMap.find( m_UniqueTransID ) != m_TransMap.end() ) {
        ++m_UniqueTransID;

        if ( m_UniqueTransID == 0 ) {
            m_UniqueTransID = 1;
        }
    }

    TransID id = m_UniqueTransID;
    t->setTransID( id );
    t->setManager( this );
    m_TransMap.emplace( id, t );

    // 加入定时器
    if ( seconds != 0 ) {
        m_TimeDriver->schedule( t, seconds * 1000 );
    }

    // 自增
    ++m_UniqueTransID;

    return id;
}

Transaction * TransactionManager::get( TransID id )
{
    TransMap::iterator result;

    result = m_TransMap.find( id );
    if ( result != m_TransMap.end() ) {
        return result->second;
    }

    return nullptr;
}

bool TransactionManager::remove( Transaction * t )
{
    return remove( t->getTransID() );
}

bool TransactionManager::remove( TransID id )
{
    TransMap::iterator result;

    result = m_TransMap.find( id );
    if ( result == m_TransMap.end() ) {
        return false;
    }

    Transaction * t = result->second;
    if ( t != nullptr ) {
        // 从定时器中取消
        m_TimeDriver->cancel( t );
    }

    // 从Map中移除
    m_TransMap.erase( result );
    return true;
}

} // namespace utils
