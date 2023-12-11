
#include <algorithm>

#include "utils/integer.h"
#include "manager.h"

namespace data {

DataManager::DataManager()
    : m_Limit( 1 ),
      m_DataAgent( nullptr )
{}

DataManager::~DataManager()
{}

bool DataManager::initialize(
    uint32_t precision, uint32_t batchcount,
    RouterFunction router, ExceptionFunction exceptioner, kaguya::State * state )
{
    // 限制
    m_Limit = batchcount;
    // 创建DataAgent
    return ( m_DataAgent = IDataAgent::create( precision, router, exceptioner, state ) ) != nullptr;
}

void DataManager::finalize()
{
    flush();

    if ( m_DataAgent != nullptr ) {
        delete m_DataAgent;
        m_DataAgent = nullptr;
    }

    m_SchemeHelper.finalize();
}

void DataManager::regscheme( const SchemeHelper::Register & reg )
{
    m_SchemeHelper.initialize( m_DataAgent->prototype(), reg );
}

bool DataManager::connect( const Endpoint & ep, uint32_t timeout )
{
    return m_DataAgent->connect( ep.host, ep.port, timeout );
}

const std::string & DataManager::schemeversion() const
{
    return m_DataAgent->prototype()->schemeversion();
}

void DataManager::insert( ISQLData * block )
{
    save( DBMethod::Insert, block );
}

void DataManager::update( ISQLData * block )
{
    save( DBMethod::Update, block );
}

void DataManager::remove( ISQLData * block, bool isfree )
{
    save( DBMethod::Remove, block, isfree );
}

void DataManager::replace( ISQLData * block )
{
    save( DBMethod::Replace, block );
}

void DataManager::query( ISQLData * q, int32_t timeout, const GeneralQueryCallback & callback )
{
    auto adapter = new GeneralQueryAdapter( callback );
    if ( adapter == nullptr ) {
        return;
    }

    m_DataAgent->query( q, adapter, timeout );
}

void DataManager::query( const std::vector<ISQLData *> & qs, int32_t timeout, const GeneralQueryCallback & callback )
{
    auto adapter = new GeneralQueryAdapter( callback );
    if ( adapter == nullptr ) {
        return;
    }

    m_DataAgent->queryBatch( qs, adapter, timeout );
}

void DataManager::save( DBMethod method, ISQLData * block, bool isfree )
{
    assert( method != DBMethod::Invoke && "Invalid Method" );
    if ( method == DBMethod::Invoke ) {
        return;
    }

    // FUCK
    block->set_method( method );
    //find existed data then replace it
    auto pos = std::find_if(
            m_DataChanges.begin(), m_DataChanges.end(),
            [&] ( const auto & kv ) { return kv.first == block; } );
    if ( pos == m_DataChanges.end() ) {
        m_DataChanges.emplace_back( block, method );
    } else {
        auto m = shift_dbmethod( pos->second, method );
        m_DataChanges.erase( pos );
        if ( m != DBMethod::None ) {
            m_DataChanges.emplace_back( block, m );
        }
    }

    if ( method == DBMethod::Remove && isfree ) {
        destroy( block );
    }

    if ( m_Limit == 0 || m_DataChanges.size() < m_Limit ) return;

    // flush to db
    flush( DATA );
}

void DataManager::flush( int32_t type )
{
    if ( ( type & DATA )
        && m_DataAgent != nullptr ) {
        for ( const auto & chg : m_DataChanges ) {
            switch ( chg.second ) {
                case DBMethod::Insert: m_DataAgent->insert( chg.first, nullptr, 0 ); break;
                case DBMethod::Update: m_DataAgent->update( chg.first ); break;
                case DBMethod::Remove: m_DataAgent->remove( chg.first ); break;
                case DBMethod::Replace: m_DataAgent->replace( chg.first, nullptr, 0 ); break;
                default: break;
            }
        }

        m_DataChanges.clear();
    }

    if ( type & MEMORY ) {
        // 销毁玩家
        for ( auto block : m_DestroyDataBlock ) {
            delete block;
        }
        m_DestroyDataBlock.clear();
    }
}

void DataManager::invoke( ISQLData * block, const std::string & script, bool isreplace )
{
    // 强行刷新队列中的数据
    flush( DATA );

    block->set_method( DBMethod::Invoke );
    m_DataAgent->invoke( block, script, isreplace );
}

ISQLData * DataManager::create( const Tablename & table )
{
    return prototype()->data( table );
}

ISQLData * DataManager::parse( const Tablename & table, const Slice & slice, bool ismerge )
{
    ISQLData * data = create( table );
    if ( data == nullptr ) {
        return nullptr;
    }

    // 解码
    data->decode( slice, eCodec_Dirty );
    data->clean();
    // 合并数据
    if ( ismerge ) {
        data = m_DataAgent->merge( data );
    }

    return data;
}

bool DataManager::destroy( ISQLData * data )
{
    if ( m_DestroyDataBlock.end()
        != m_DestroyDataBlock.find( data ) ) {
        return false;
    }

    m_DestroyDataBlock.insert( data );
    return true;
}

} // namespace data
