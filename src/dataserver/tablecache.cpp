
#include <cmath>

#include "base/base.h"
#include "utils/xtime.h"
#include "scheme/sqlbind.h"
#include "extra/ticksvc.h"
#include "message/protocol.h"

#include "config.h"
#include "dbthreads.h"

#include "tablecache.h"

namespace data {

DataBlock::DataBlock( const Tablename & table )
    : m_DataBlock( nullptr ),
      m_Timestamp( 0LL )
{}

DataBlock::~DataBlock()
{
    if ( m_DataBlock != nullptr ) {
        delete m_DataBlock;
        m_DataBlock = nullptr;
    }

    if ( isSchedule() ) {
        g_TickService->cancel( this );
    }
}

void DataBlock::onEnd()
{
    if ( getMethod() == DBMethod::Remove ) {
        delete this;
    }
}

bool DataBlock::onTimer( uint32_t & timeout )
{
    return !flush2DB();
}

void DataBlock::updateMethod( DBMethod method )
{
    if ( method == DBMethod::Invoke ) {
        method = DBMethod::Update;
    }

    setMethod( shift_dbmethod( getMethod(), method ) );
}

void DataBlock::schedule( int32_t interval )
{
    int64_t now = utils::time();

    // 更新时间戳
    if ( m_Timestamp == 0 ) {
        m_Timestamp = now;
    }

    if ( now - m_Timestamp > g_AppConfig.getCacheLifetime() ) {
        // 缓存的时间太长了, 强制刷新到数据库中
        flush2DB();
        g_TickService->cancel( this );
    } else {
        // 启动定时器
        g_TickService->schedule( this, interval );
    }
}

bool DataBlock::flush2DB( TransID transid )
{
    std::string sqlcmd;
    std::vector<std::string> escapevalues;

    switch ( getMethod() ) {
        case DBMethod::Insert:
            m_DataBlock->insert( sqlcmd, escapevalues );
            break;
        case DBMethod::Update:
            m_DataBlock->update( sqlcmd, escapevalues );
            break;
        case DBMethod::Remove:
            m_DataBlock->remove( sqlcmd, escapevalues );
            break;
        case DBMethod::Replace:
            m_DataBlock->replace( sqlcmd, escapevalues );
            break;
        case DBMethod::None: {
            m_Timestamp = 0;
            m_DataBlock->clean();
            return true;
        } break;

        default:
            break;
    }

    if ( g_DBThreads->post(
             m_DataBlock->tablename(), getIdxString(), getMethod(), transid, std::move(sqlcmd), std::move(escapevalues) ) ) {
        // 清空
        m_DataBlock->clean();
        //
        m_Timestamp = 0;
        setMethod( DBMethod::None );
        return true;
    }

    return false;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

IndexEntry::IndexEntry( const std::string & idxstr, const Tablename & table )
    : m_Status( eStatus_InComplete ),
      m_Index( idxstr ),
      m_Table( table )
{}

IndexEntry::~IndexEntry()
{}

void IndexEntry::add( const std::string & key )
{
    if ( m_Index.empty() ) {
        return;
    }

    if ( m_DataKeys.end()
        == m_DataKeys.find( key ) ) {
        m_DataKeys.insert( key );
    }
}

void IndexEntry::del( const std::string & key )
{
    if ( m_Index.empty() ) {
        return;
    }

    auto result = m_DataKeys.find( key );
    if ( result != m_DataKeys.end() ) {
        m_DataKeys.erase( result );
    }
}

bool IndexEntry::has( const std::string & key ) const
{
    return m_DataKeys.find( key ) != m_DataKeys.end();
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

TableCache::TableCache( const Tablename & table, bool iscomplete )
    : m_Table( table ),
      m_IsComplete( iscomplete )
{
    m_DataCache.rehash( std::ceil( eMaxBucketCount / m_DataCache.max_load_factor() ) );
    m_IndexCache.rehash( std::ceil( eMaxBucketCount / m_IndexCache.max_load_factor() ) );
}

TableCache::~TableCache()
{
    for ( const auto & it : m_DataCache ) {
        delete it.second;
    }

    for ( const auto & it : m_IndexCache ) {
        delete it.second;
    }

    for ( const auto & it : m_MessageQueueMap ) {
        for ( const auto & msg : it.second ) {
            delete msg;
        }
    }

    m_DataCache.clear();
    m_IndexCache.clear();
    m_MessageQueueMap.clear();
}

void TableCache::flush2DB()
{
    size_t flushcount = 0;
    int64_t now = utils::now();

    for ( const auto & it : m_DataCache ) {
        DataBlock * block = it.second;

        if ( block != nullptr
            && block->getMethod() != DBMethod::None ) {
            ++flushcount;
            block->flush2DB();
            g_TickService->cancel( block );
        }
    }

    LOG_TRACE( "TableCache::flush2DB() : flush {} datablock(s) to the MySQLTable:'{}', Elapsed:{} msec(s) .\n",
        flushcount, m_Table, utils::now() - now );
}

void TableCache::datasets( Slices & datas, const ISQLData * query )
{
    for ( const auto & it : m_DataCache ) {
        DataBlock * block = it.second;
        if ( block != nullptr
            && ( query == nullptr || query->match( block->getDataBlock() ) ) ) {
            datas.push_back( block->getDataBlock()->encode( eCodec_All ) );
        }
    }
}

void TableCache::destroyBlock( const std::string & key, DataBlock * block )
{
    // 刷新到数据库中
    if ( block->getMethod() != DBMethod::None ) {
        block->flush2DB();
    }

    // 从缓存中删除
    removeBlock( key );
    delete block;
}

DataBlock * TableCache::createBlock( ISQLData * data, const std::string & key )
{
    DataBlock * block = new DataBlock( m_Table );

    // 初始化数据
    if ( block != nullptr ) {
        // 数据块+索引字符串
        block->setDataBlock( data );
        // 添加到缓存中
        m_DataCache.emplace( key, block );
    }

    return block;
}

void TableCache::removeBlock( const std::string & key )
{
    m_DataCache.erase( key );
}

DataBlock * TableCache::getBlock( const std::string & key ) const
{
    auto result = m_DataCache.find( key );
    if ( result != m_DataCache.end() ) {
        return result->second;
    }

    return nullptr;
}

bool TableCache::isEmpty( const std::string & key ) const
{
    auto result = m_MessageQueueMap.find( key );
    if ( result == m_MessageQueueMap.end() ) {
        return true;
    }

    return result->second.empty();
}

void TableCache::pushMessage( const std::string & key, SSMessage * msg )
{
    auto result = m_MessageQueueMap.find( key );
    if ( result != m_MessageQueueMap.end() ) {
        result->second.push_back( msg );
    } else {
        MessageQueue mq;
        mq.push_back( msg );
        m_MessageQueueMap.emplace( key, std::move(mq) );
    }
}

void TableCache::popMessages( const std::string & key, MessageQueue & msgqueue )
{
    auto result = m_MessageQueueMap.find( key );
    if ( result == m_MessageQueueMap.end() ) {
        return;
    }

    msgqueue.swap( result->second );
    m_MessageQueueMap.erase( result );
}

void TableCache::removeEntry( const std::string & idx )
{
    m_IndexCache.erase( idx );
}

IndexEntry * TableCache::getEntry( const std::string & idx ) const
{
    auto result = m_IndexCache.find( idx );
    if ( result != m_IndexCache.end() ) {
        return result->second;
    }

    return nullptr;
}

IndexEntry * TableCache::createEntry( const std::string & idx )
{
    IndexEntry * entry = getEntry( idx );
    if ( entry != nullptr ) {
        return entry;
    }

    entry = new IndexEntry( idx, m_Table );
    if ( entry == nullptr ) {
        return nullptr;
    }

    // 添加
    m_IndexCache.emplace( idx, entry );

    // 判断是否完整
    if ( !idx.empty() && m_IsComplete ) {
        entry->setStatus( IndexEntry::eStatus_Complete );
    }

    return entry;
}

bool TableCache::isException( const std::string & idxstr ) const
{
    return m_ExceptionTable.end()
        != m_ExceptionTable.find( idxstr );
}

void TableCache::throwException( const std::string & idxstr )
{
    auto result = m_ExceptionTable.find( idxstr );
    if ( result != m_ExceptionTable.end() ) {
        m_ExceptionTable.erase( result );
    }

    m_ExceptionTable.emplace( idxstr, utils::time() );
}

void TableCache::fixException( const std::string & idxstr )
{
    auto result = m_ExceptionTable.find( idxstr );
    if ( result != m_ExceptionTable.end() ) {
        m_ExceptionTable.erase( result );
    }
}

} // namespace data
