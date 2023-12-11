
#include <algorithm>

#include "base/base.h"
#include "base/database.h"
#include "base/limits.h"
#include "utils/xtime.h"

#include "timetick.h"
#include "resultqueue.h"
#include "transaction.h"
#include "agentclient.h"

#include "scheme/sqlbind.h"
#include "message/dbprotocol.h"

#include "agentimpl.h"

namespace data {

DataAgentImpl::DataAgentImpl( uint32_t precision, RouterFunction routerfunc, ExceptionFunction exceptionfunc, kaguya::State * state )
    : m_Timeout( 0x7fffffff ),
      m_TimeTick( nullptr ),
      m_ProtoType( nullptr ),
      m_QueryCache( nullptr ),
      m_AgentClient( nullptr ),
      m_ResultQueue( nullptr ),
      m_TransManager( nullptr ),
      m_RouterFunction( routerfunc ),
      m_ExceptionFunction( exceptionfunc )
{
    // 初始化表结构
    m_ProtoType = new TableProtoType( state );
    assert( m_ProtoType != nullptr && "create TableProtoType failed" );
    m_ProtoType->initialize();

    // 结果队列
    m_ResultQueue = new ResultQueue();
    assert( m_ResultQueue != nullptr && "create ResultQueue failed" );

    // 定时器
    m_TimeTick = new TimeTick( precision );
    assert( m_TimeTick != nullptr && "create TimeTick failed" );

    // 事务管理器
    m_TransManager = new utils::TransactionManager( m_TimeTick->getTimer() );
    assert( m_TransManager != nullptr && "create utils::TransactionManager failed" );

    // 网络线程
    m_AgentClient = new AgentClient( this );
    assert( m_AgentClient != nullptr && "create AgentClient failed" );

    // 查询缓存
    m_QueryCache = new QueryCache( m_ProtoType, m_TransManager );
    assert( m_QueryCache != nullptr && "create QueryCache failed" );
}

DataAgentImpl::~DataAgentImpl()
{
    // 检查连接是否正常
    // 如果不正常的情况下挂起
    while ( !isAlive() ) {
        LOG_ERROR( "the DataAgent Wait for the DataServer(s) to launch ...\n" );

        m_ResultQueue->process( this, true );
        utils::TimeUtils::sleep( 500 );
    }

    // 停止网络线程
    if ( m_AgentClient != nullptr ) {
        m_AgentClient->halt();
        m_AgentClient->stop();
        delete m_AgentClient;
        m_AgentClient = nullptr;
    }

    if ( m_TransManager != nullptr ) {
        delete m_TransManager;
        m_TransManager = nullptr;
    }

    if ( m_TimeTick != nullptr ) {
        m_TimeTick->stop();
        delete m_TimeTick;
        m_TimeTick = nullptr;
    }

    if ( m_ResultQueue != nullptr ) {
        delete m_ResultQueue;
        m_ResultQueue = nullptr;
    }

    if ( m_ProtoType != nullptr ) {
        m_ProtoType->finalize();
        delete m_ProtoType;
        m_ProtoType = nullptr;
    }

    if ( m_QueryCache != nullptr ) {
        delete m_QueryCache;
        m_QueryCache = nullptr;
    }

    m_DataHostList.clear();
}

bool DataAgentImpl::check( int64_t now ) const
{
    if ( now == 0 ) {
        now = utils::TimeUtils::time( true );
    }

    for ( const auto & dh : m_DataHostList ) {
        if ( now - dh.keepalive > m_Timeout * 10 ) {
            return false;
        }
    }

    return true;
}

ISQLData * DataAgentImpl::merge( ISQLData * data )
{
    std::string keystr;
    data->keystring( keystr );

    data = m_QueryCache->mergedirty(
        data->tablename(), keystr, data );
    if ( data != nullptr ) {
        data->clean();
    }

    return data;
}

void DataAgentImpl::schedule( int64_t now )
{
    // 处理结果队列
    m_ResultQueue->process( this );

    // 定时器调度
    m_TimeTick->run( now );
}

bool DataAgentImpl::connect( const std::string & host, uint16_t port, uint32_t timeout )
{
    // 设置全局超时时间
    m_Timeout = std::min( m_Timeout, timeout );

    // 初始化数据代理
    // 会默认重试10次，相当于超时时间的10倍
    sid_t sid = m_AgentClient->connect(
        host.c_str(), port, timeout * 10 );
    if ( sid != ( sid_t )( -1 ) ) {
        m_DataHostList.push_back(
            DataHost( sid, 1, utils::TimeUtils::time( true ) ) );
        return true;
    }

    return false;
}

bool DataAgentImpl::insert( ISQLData * d, IDataAdapter * adapter, uint32_t timeout )
{
    TransID transid = 0;

    // 检查输入参数
    assert( d != nullptr && "DataBlock Invalid" );
    if ( d == nullptr ) {
        return false;
    }

    if ( adapter != nullptr && timeout > 0 ) {
        DataAutoIncrementTrans * t = new DataAutoIncrementTrans;
        assert( t != nullptr && "create DataAutoIncrementTrans failed" );
        t->setAdapter( adapter );
        m_TransManager->append( t, timeout );

        // 获取新创建的事务ID
        transid = t->getTransID();
    }

    // 获取查询索引
    std::string keystr, idxstr;
    d->keystring( keystr );
    d->indexstring( eCodec_All, idxstr );

    // 发送插入指令
    DBInsertCommand cmd;
    cmd.transid = transid;
    cmd.table = d->tablename();
    cmd.timeout = timeout;
    cmd.dbdata = d->encode( eCodec_Dirty );
    m_AgentClient->send( route2DataHost( d->tablename(), idxstr ), &cmd );

    // 该索引正在被查询, 添加到脏数据缓存中
    if ( isQuery( d->tablename(), idxstr ) ) {
        m_QueryCache->pushdirty( keystr, idxstr, d );
    }
    // 清空
    d->clean();

    return true;
}

bool DataAgentImpl::update( ISQLData * d )
{
    // 检查输入参数
    assert( d != nullptr && !d->isKeydirty() );
    if ( d == nullptr
        || ( d != nullptr && d->isKeydirty() ) ) {
        return false;
    }

    if ( !d->isDirty() ) {
        return true;
    }

    // 主键字符串
    std::string keystr, idxstr;
    d->keystring( keystr );
    d->indexstring( eCodec_All, idxstr );

    // 发送更新指令
    DBUpdateCommand cmd;
    cmd.table = d->tablename();
    cmd.keystr = keystr;
    cmd.dbdata = d->encode( eCodec_Dirty );
    m_AgentClient->send( route2DataHost( d->tablename(), idxstr ), &cmd );

    // 该索引正在被查询, 添加到脏数据缓存中
    if ( isQuery( d->tablename(), idxstr ) ) {
        m_QueryCache->pushdirty( keystr, idxstr, d );
    }
    // 清空
    d->clean();

    return true;
}

bool DataAgentImpl::remove( ISQLData * d )
{
    // 检查输入参数
    assert( d != nullptr && "DataBlock Invalid" );
    if ( d == nullptr ) {
        return false;
    }

    // 主键字符串
    std::string keystr, idxstr;
    d->keystring( keystr );
    d->indexstring( eCodec_All, idxstr );

    // 发送删除指令
    DBRemoveCommand cmd;
    cmd.table = d->tablename();
    cmd.keystr = keystr;
    cmd.dbdata = d->encode( eCodec_Dirty );
    m_AgentClient->send( route2DataHost( d->tablename(), idxstr ), &cmd );

    // 该索引正在被查询, 添加到脏数据缓存中
    if ( isQuery( d->tablename(), idxstr ) ) {
        m_QueryCache->pushdirty( keystr, idxstr, d );
    }
    // 清空
    d->clean();

    return true;
}

bool DataAgentImpl::replace( ISQLData * d, IDataAdapter * adapter, uint32_t timeout )
{
    TransID transid = 0;

    // 检查输入参数
    assert( d != nullptr && "DataBlock Invalid" );
    if ( d == nullptr ) {
        return false;
    }

    if ( adapter != nullptr && timeout > 0 ) {
        DataAutoIncrementTrans * t = new DataAutoIncrementTrans;
        assert( t != nullptr && "create DataAutoIncrementTrans failed" );
        t->setAdapter( adapter );
        m_TransManager->append( t, timeout );

        // 获取新创建的事务ID
        transid = t->getTransID();
    }

    // 查询索引
    std::string keystr, idxstr;
    d->keystring( keystr );
    d->indexstring( eCodec_All, idxstr );

    // 发送替换指令
    DBReplaceCommand cmd;
    cmd.transid = transid;
    cmd.table = d->tablename();
    cmd.timeout = timeout;
    cmd.dbdata = d->encode( eCodec_Dirty );
    m_AgentClient->send( route2DataHost( d->tablename(), idxstr ), &cmd );

    // 该索引正在被查询, 添加到脏数据缓存中
    if ( isQuery( d->tablename(), idxstr ) ) {
        m_QueryCache->pushdirty( keystr, idxstr, d );
    }
    // 清空
    d->clean();

    return true;
}

bool DataAgentImpl::isQuery( const Tablename & table, const std::string & idxstr )
{
    return m_QueryCache->isQuery( table, idxstr );
}

bool DataAgentImpl::query( ISQLData * q, IDataAdapter * adapter, uint32_t timeout )
{
    // 检查输入参数
    assert( q != nullptr
        && adapter != nullptr && timeout > 0 );
    if ( q == nullptr
        || adapter == nullptr || timeout == 0 ) {
        return false;
    }

    // 索引字符串
    std::string key;
    q->indexstring( eCodec_Query, key );

    // 创建事务
    DataResultTrans * t = new DataResultTrans;
    assert( t != nullptr && "create DataResultTrans failed" );
    t->setAgent( this );
    t->setAdapter( adapter );
    t->addIndexString( q->tablename(), key );
    m_TransManager->append( t, timeout );

    // 正在查询...
    // 等待其他数据返回
    if ( !isQuery( q->tablename(), key ) ) {
        // 发送查询命令
        DBQueryCommand cmd;
        cmd.indexstr = key;
        cmd.dbdata = q->encode( eCodec_Query );
        cmd.table = q->tablename();
        cmd.transid = t->getTransID();
        cmd.timeout = timeout;
        m_AgentClient->send( route2DataHost( q->tablename(), key ), &cmd );
    }

    // 添加到查询缓存中
    m_QueryCache->inqueue( q->tablename(), key, t->getTransID() );

    return true;
}

bool DataAgentImpl::queryBatch( const std::vector<ISQLData *> & qs, IDataAdapter * adapter, uint32_t timeout )
{
    // 检查输入参数
    assert( adapter != nullptr && timeout > 0 );
    if ( adapter == nullptr || timeout == 0 ) {
        return false;
    }

    if ( qs.empty() ) {
        adapter->complete();
        return false;
    }

    // 创建事务
    // 事务ID
    DataResultTrans * t = new DataResultTrans;
    assert( t != nullptr && "create DataResultTrans failed" );
    t->setAgent( this );
    t->setAdapter( adapter );
    t->setRef( (int32_t)qs.size() );
    m_TransManager->append( t, timeout );

    std::vector<TableIndexEntry> entries;
    std::vector<std::pair<sid_t, DBQueryBatchCommand *>> cmds;

    // 预分配内存
    cmds.reserve( m_DataHostList.size() );
    // 批量查询
    for ( const auto & q : qs ) {
        QueryUnit u;
        u.table = q->tablename();
        u.dbdata = q->encode( eCodec_Query );
        q->indexstring( eCodec_Query, u.indexstr );

        TableIndexEntry entry( u.table, u.indexstr );
        if ( entries.end()
            == std::find( entries.begin(), entries.end(), entry ) ) {
            entries.push_back( entry );
        }

        if ( !isQuery( u.table, u.indexstr ) ) {
            size_t j = 0;
            sid_t sid = route2DataHost( u.table, u.indexstr );
            for ( j = 0; j < cmds.size(); ++j ) {
                if ( cmds[j].first == sid ) {
                    cmds[j].second->querylist.push_back( u );
                    break;
                }
            }
            if ( j == cmds.size() ) {
                DBQueryBatchCommand * cmd = new DBQueryBatchCommand;
                assert( cmd != nullptr );
                cmd->timeout = timeout;
                cmd->querylist.push_back( u );
                cmd->transid = t->getTransID();
                cmds.emplace_back( sid, cmd );
            }
        }

        // 添加到查询缓存中
        t->addIndexString( u.table, u.indexstr );
        m_QueryCache->inqueue( u.table, u.indexstr, t->getTransID() );
    }

    if ( qs.size() != entries.size() ) {
        t->setRef( entries.size() );
        // TODO: 提示出错
    }

    for ( auto & cmd : cmds ) {
        // 发送查询命令
        m_AgentClient->send( cmd.first, cmd.second );
        delete cmd.second;
    }

    return true;
}

bool DataAgentImpl::invoke( ISQLData * d, const std::string & script, bool isreplace )
{
    // 检查输入参数
    assert( d != nullptr && "DataBlock Invalid" );
    if ( d == nullptr ) {
        return false;
    }

    // 主键字符串
    std::string keystr, idxstr;
    d->keystring( keystr );
    d->indexstring( eCodec_All, idxstr );

    // 发送删除指令
    DBInvokeCommand cmd;
    cmd.table = d->tablename();
    cmd.keystr = keystr;
    cmd.dbdata = d->encode( eCodec_Dirty );
    cmd.script = script;
    cmd.isreplace = isreplace;
    m_AgentClient->send( route2DataHost( d->tablename(), idxstr ), &cmd );

    // 该索引正在被查询, 添加到脏数据缓存中
    if ( isQuery( d->tablename(), idxstr ) ) {
        m_QueryCache->pushdirty( keystr, idxstr, d, script, isreplace );
    }
    // 清空
    d->clean();

    return true;
}

bool DataAgentImpl::flush( const std::vector<ISQLData *> & qs, bool isdrop )
{
    std::vector<std::pair<sid_t, DBFlushCommand>> cmds;

    // 预分配内存
    cmds.reserve( m_DataHostList.size() );
    // 拼接索引
    for ( const auto & q : qs ) {
        TableIndexEntry entry;
        entry.table = q->tablename();
        q->indexstring( eCodec_All, entry.indexstr );

        size_t j = 0;
        sid_t sid = route2DataHost(
            entry.table, entry.indexstr );
        for ( j = 0; j < cmds.size(); ++j ) {
            if ( cmds[j].first == sid ) {
                cmds[j].second.entrylist.push_back( entry );
                break;
            }
        }
        if ( j == cmds.size() ) {
            DBFlushCommand cmd;
            // 是否丢弃服务器中的内存数据
            cmd.dropflag = isdrop ? 1 : 0;
            cmd.entrylist.push_back( entry );
            cmds.emplace_back( sid, cmd );
        }
    }

    // 发送
    for ( auto & cmd : cmds ) {
        m_AgentClient->send( cmd.first, &cmd.second );
    }

    return false;
}

bool DataAgentImpl::failover( const std::vector<ISQLData *> & qs )
{
    std::vector<std::pair<sid_t, DBFailoverCommand>> cmds;

    // 预分配内存
    cmds.reserve( m_DataHostList.size() );
    // 拼接索引
    for ( const auto & q : qs ) {
        TableIndexEntry entry;
        entry.table = q->tablename();
        q->indexstring( eCodec_All, entry.indexstr );

        size_t j = 0;
        sid_t sid = route2DataHost(
            entry.table, entry.indexstr );
        for ( j = 0; j < cmds.size(); ++j ) {
            if ( cmds[j].first == sid ) {
                cmds[j].second.exceptions.push_back( entry );
                break;
            }
        }
        if ( j == cmds.size() ) {
            DBFailoverCommand cmd;
            // 是否丢弃服务器中的内存数据
            cmd.exceptions.push_back( entry );
            cmds.emplace_back( sid, cmd );
        }
    }

    // 发送
    for ( size_t i = 0; i < cmds.size(); ++i ) {
        m_AgentClient->send( cmds[i].first, &( cmds[i].second ) );
    }

    return false;
}

bool DataAgentImpl::failover( const std::vector<TableIndexEntry> & qs )
{
    std::vector<std::pair<sid_t, DBFailoverCommand>> cmds;

    // 预分配内存
    cmds.reserve( m_DataHostList.size() );
    // 拼接索引
    for ( const auto & q : qs ) {
        size_t j = 0;
        sid_t sid = route2DataHost( q.table, q.indexstr );

        for ( j = 0; j < cmds.size(); ++j ) {
            if ( cmds[j].first == sid ) {
                cmds[j].second.exceptions.push_back( q );
                break;
            }
        }
        if ( j == cmds.size() ) {
            DBFailoverCommand cmd;
            // 是否丢弃服务器中的内存数据
            cmd.exceptions.push_back( q );
            cmds.emplace_back( sid, cmd );
        }
    }

    // 发送
    for ( size_t i = 0; i < cmds.size(); ++i ) {
        m_AgentClient->send( cmds[i].first, &( cmds[i].second ) );
    }

    return false;
}

void DataAgentImpl::updateKeepalive( sid_t sid, uint8_t status )
{
    for ( auto & dh : m_DataHostList ) {
        if ( dh.sid == sid ) {
            dh.status = status;
            if ( status == 1 ) dh.keepalive = utils::TimeUtils::time( true );
            break;
        }
    }
}

const std::string & DataAgentImpl::schemeversion() const
{
    return m_ProtoType->schemeversion();
}

bool DataAgentImpl::isAlive() const
{
    for ( auto & dh : m_DataHostList ) {
        if ( dh.status == 0 ) {
            return false;
        }
    }

    return true;
}

sid_t DataAgentImpl::route2DataHost( const Tablename & table, const std::string & idxstr ) const
{
    assert( !m_DataHostList.empty() && "DataServerList is Empty" );

    if ( m_DataHostList.size() == 1
        || m_RouterFunction == nullptr ) {
        return m_DataHostList.begin()->sid;
    }

    uint8_t index = m_RouterFunction(
        m_DataHostList.size(), table, idxstr );
    assert( index < m_DataHostList.size() && "Router Index Invalid" );

    if ( index < m_DataHostList.size() ) {
        return m_DataHostList[index].sid;
    }

    return 0;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

IDataAgent * IDataAgent::create( uint32_t precision, RouterFunction routerfunc, ExceptionFunction exceptionfunc, kaguya::State * state )
{
    DataAgentImpl * agent = new DataAgentImpl(
        precision, routerfunc, exceptionfunc, state );
    assert( agent != nullptr && "create DataAgentImpl failed" );

    // 初始化定时器
    if ( !agent->getTimeTick()->start() ) {
        delete agent;
        return nullptr;
    }

    if ( !agent->getAgentClient()->start() ) {
        delete agent;
        return nullptr;
    }

    return agent;
}

} // namespace data
