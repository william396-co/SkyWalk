
#include <cstdlib>
#include "utils/file.h"

#include "scheme/sqlbind.h"
#include "message/dbprotocol.h"

#include "dataagent/agent.h"

#include "agentimpl.h"
#include "transaction.h"

namespace data {

DataResultTrans::DataResultTrans()
    : m_Ref( 1 ),
      m_Adapter( nullptr ),
      m_Agent( nullptr )
{}

DataResultTrans::~DataResultTrans()
{}

void DataResultTrans::onTimeout()
{
    m_Adapter->timeout();
    m_Adapter = nullptr;

    for ( const auto & qi : m_IndexStringList ) {
        m_Agent->getQueryCache()->outqueue(
            qi.first, qi.second, getTransID() );
    }
}

void DataResultTrans::onTrigger( void * data )
{
    if ( data != nullptr ) {
        Slices results;
        std::vector<ISQLData *> dirtylist;
        DBResultsCommand * cmd = (DBResultsCommand *)data;

        // 回调结果
        m_Adapter->datasets( cmd->table, cmd->results );

        // NOTICE: 回调查询过程中增加的数据集合
        // 本版本为了保持接口一致,
        // 所以采用了 "先序列化为slice再由逻辑层反序列化为ISQLData" 的方案
        m_Agent->getQueryCache()->popdirty( cmd->table, cmd->indexstr, dirtylist );
        // encode()
        for ( auto & d : dirtylist ) {
            results.push_back( d->encode( eCodec_Dirty ) );
            delete d;
        }
        // datasets()
        if ( !results.empty() ) {
            // 回调结果
            m_Adapter->datasets( cmd->table, results );
            // 销毁
            for ( const auto & r : results ) {
                std::free( (void *)r.data() );
            }
        }

        // 清空查询缓存
        m_Agent->getQueryCache()->clearqueue( cmd->table, cmd->indexstr, getTransID() );
    }

    // 计数器
    if ( --m_Ref <= 0 ) {
        m_Adapter->complete();
        m_Adapter = nullptr;
        delete this;
    }
}

DataAutoIncrementTrans::DataAutoIncrementTrans()
    : m_Adapter( nullptr )
{}

DataAutoIncrementTrans::~DataAutoIncrementTrans()
{}

void DataAutoIncrementTrans::onTimeout()
{
    m_Adapter->timeout();
    m_Adapter = nullptr;
}

void DataAutoIncrementTrans::onTrigger( void * data )
{
    DBAutoIncreaseCommand * command = (DBAutoIncreaseCommand *)data;

    m_Adapter->autoincr( command->lastid );
    m_Adapter->complete();
    m_Adapter = nullptr;

    delete this;
}

} // namespace data
