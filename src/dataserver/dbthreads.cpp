
#include <cstring>

#include "utils/xtime.h"
#include "utils/utility.h"
#include "utils/hashfunc.h"

#include "base/base.h"
#include "mysql/result.h"
#include "message/dbprotocol.h"
#include "io/service.h"

#include "task.h"
#include "config.h"
#include "reservequeue.h"

#include "dbthreads.h"

namespace data {

QueryResultsHandler::QueryResultsHandler( sid_t sid, TransID transid, const std::string & idxstr, ISQLData * prototype )
    : m_Sid( sid ),
      m_TransID( transid ),
      m_IndexString( idxstr ),
      m_ProtoType( prototype )
{}

QueryResultsHandler::~QueryResultsHandler()
{
    for ( const auto & s : m_Results ) {
        std::free( (void *)( s.data() ) );
    }

    delete m_ProtoType;
    m_ProtoType = nullptr;
    m_Results.clear();
    m_BlockList.clear();
}

void QueryResultsHandler::reserve( size_t capacity )
{
    m_Results.reserve( capacity );
    m_BlockList.reserve( capacity );
}

void QueryResultsHandler::process( const Slices & result )
{
    ISQLData * data = m_ProtoType->clone( false );
    if ( data == nullptr ) {
        return;
    }

    // 解析数据结果
    data->parse( result );

    //
    m_BlockList.push_back( data );
    // 匹配的情况下放入结果集中
    if ( m_ProtoType->match( data ) ) {
        m_Results.push_back( data->encode( eCodec_All ) );
    }
}

void QueryResultsHandler::send()
{
    if ( m_Sid == 0
        || m_TransID == 0 ) {
        return;
    }

    // 返回给客户端
    DBResultsCommand command;
    command.table = m_ProtoType->tablename();
    command.transid = m_TransID;
    command.indexstr = m_IndexString;
    command.results.swap( m_Results );
    g_HarborService->send( m_Sid, &command );
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

DBThread::DBThread( uint8_t index )
    : m_Index( index ),
      m_Engine( nullptr ),
      m_ReserveQueue( nullptr ),
      m_KeepaliveTime( 0ULL )
{}

DBThread::~DBThread()
{}

bool DBThread::onStart()
{
    // 初始化DBEngine
    m_Engine = new MysqlEngine(
        g_AppConfig.getDBHost(), g_AppConfig.getDBPort() );
    if ( m_Engine == nullptr ) {
        return false;
    }

    // 初始化指令缓存
    m_ReserveQueue = new ReserveQueue( m_Index );
    assert( m_ReserveQueue != nullptr && "create ReserveQueue failed" );

    // 设置DBEngine参数
    m_Engine->selectdb( g_AppConfig.getDatabase() );
    m_Engine->setCharsets( g_AppConfig.getDBCharsets() );
    m_Engine->setToken(
        g_AppConfig.getDBUsername(), g_AppConfig.getDBPassword() );

    int32_t ntry = 0;
    do {
        if ( m_Engine->initialize() ) {
            break;
        }

        LOG_FATAL( "DBThread(Index:{}, Database:'{}') initialize failed, try again ...\n",
            m_Index, g_AppConfig.getDatabase() );
        utils::TimeUtils::sleep( 200 );
    } while ( ++ntry < 50 );

    if ( m_Engine->handler() == nullptr ) {
        return false;
    }

    // 初始化Keepalive时间戳
    m_KeepaliveTime = utils::now();

    std::string client_version;
    m_Engine->version( client_version );
    LOG_INFO( "DBThread(Index:{}, Database:'{}', Version:'{}') initialize succeed .\n", m_Index, g_AppConfig.getDatabase(), client_version );

    return true;
}

void DBThread::onExecute()
{
    int64_t now = utils::now();

    // 查询数据库
    process();

    // MYSQL保活功能
    if ( now - m_KeepaliveTime >= eEngine_KeepaliveSeconds ) {
        m_KeepaliveTime = now;
        m_Engine->keepalive();
    }

    // 防止空转
    int64_t used_msecs = utils::now() - now;
    if ( used_msecs >= 0 && used_msecs < eEachFrameSeconds ) {
        utils::TimeUtils::sleep( eEachFrameSeconds - used_msecs );
    }
}

void DBThread::onStop()
{
    LOG_INFO( "DBThread(Index:{}, Database:'{}') stop ...\n", m_Index, g_AppConfig.getDatabase() );

    if ( m_Engine->handler() != nullptr ) {
        process();
    }

    if ( m_ReserveQueue != nullptr ) {
        delete m_ReserveQueue;
        m_ReserveQueue = nullptr;
    }

    if ( m_Engine != nullptr ) {
        m_Engine->finalize();
        delete m_Engine;
        m_Engine = nullptr;
    }
}

bool DBThread::push2Database( DBCommand * command )
{
    if ( !isActive() ) {
        return false;
    }

    m_Lock.lock();
    m_Queue.push_back( command );
    m_Lock.unlock();

    return true;
}

void DBThread::process()
{
    std::deque<DBCommand *> taskqueue;

    // 刷新缓存中的SQL
    m_ReserveQueue->flush2DB( m_Engine );

    // 交换任务队列
    m_Lock.lock();
    std::swap( m_Queue, taskqueue );
    m_Lock.unlock();

    // 处理任务
    for ( auto cmd : taskqueue ) {
        switch ( cmd->cmd ) {
            case DBMethod::Insert:
                insert( cmd->table, cmd->idxstr, cmd->transid, cmd->command.cmd, cmd->command.params );
                break;
            case DBMethod::Query:
                query( cmd->table, cmd->idxstr, cmd->transid, cmd->command.cmd, cmd->command.params, cmd->handler );
                break;
            case DBMethod::Update:
                update( cmd->table, cmd->idxstr, cmd->command.cmd, cmd->command.params );
                break;
            case DBMethod::Remove:
                remove( cmd->table, cmd->idxstr, cmd->command.cmd, cmd->command.params );
                break;
            case DBMethod::Replace:
                replace( cmd->table, cmd->idxstr, cmd->transid, cmd->command.cmd, cmd->command.params );
                break;
            case DBMethod::Failover:
                failover( cmd->table, cmd->idxstr );
                break;
            default:
                break;
        }

        delete cmd;
    }
}

void DBThread::insert( const Tablename & table,
    const std::string & idxstr, uint32_t transid,
    const std::string & sqlcmd, const std::vector<std::string> & values )
{
    uint64_t insertid = 0;

    // fullcmd
    std::string fullsqlcmd;
    MysqlEngine::escapesqlcmd( fullsqlcmd, m_Engine, sqlcmd, values );

    // 队列有数据的情况下添加到队列中
    if ( m_ReserveQueue->isException( table, idxstr ) ) {
        m_ReserveQueue->append( table, idxstr, fullsqlcmd );
        return;
    }

    bool rc = m_Engine->insert( fullsqlcmd, insertid );
    if ( !rc ) {
        if ( !g_AppConfig.isIgnoreErrorcode( m_Engine->errorcode() ) ) {
            m_ReserveQueue->append( table, idxstr, fullsqlcmd );
        }
    } else if ( transid != 0 ) {
        POST<AutoIncreaseTask>(
            eTaskType_AutoIncrease,
            new AutoIncreaseTask( table, transid, std::to_string( insertid ) ) );
    }
}

void DBThread::query( const Tablename & table,
    const std::string & idxstr, uint32_t transid,
    const std::string & sqlcmd, const std::vector<std::string> & values, QueryResultsHandler * handler )
{
    // fullcmd
    std::string fullsqlcmd;
    MysqlEngine::escapesqlcmd( fullsqlcmd, m_Engine, sqlcmd, values );

    // 队列有数据的情况下直接报错
    if ( m_ReserveQueue->isException( table, idxstr ) ) {
        LOG_ERROR( "DBThread::query(SQLCMD:'{}') : the DBThread::ReserveQueue is NOT EMPTY .\n", fullsqlcmd );
        return;
    }

    bool rc = false;

    if ( handler == nullptr ) {
        DatablockTask * task = new DatablockTask( table, transid );
        assert( task != nullptr && "create DatablockTask failed" );

        rc = m_Engine->query( fullsqlcmd, task->results );
        if ( !rc ) {
            delete task;
        } else {
            POST<DatablockTask>( eTaskType_Datablock, task );
        }
    } else {
        rc = m_Engine->query( fullsqlcmd, handler );

        if ( rc ) {
            // 发送结果给客户端
            handler->send();

            // 创建ResultTask
            ResultsTask * task = new ResultsTask( table, transid );
            assert( task != nullptr && "create ResultsTask failed" );
            task->results.swap( handler->blocks() );
            POST<ResultsTask>( eTaskType_Results, task );
        }

        delete handler;
    }
}

void DBThread::update(
    const Tablename & table, const std::string & idxstr,
    const std::string & sqlcmd, const std::vector<std::string> & values )
{
    uint32_t naffected = 0;

    // fullcmd
    std::string fullsqlcmd;
    MysqlEngine::escapesqlcmd( fullsqlcmd, m_Engine, sqlcmd, values );

    // 队列有数据的情况下添加到队列中
    if ( m_ReserveQueue->isException( table, idxstr ) ) {
        m_ReserveQueue->append( table, idxstr, fullsqlcmd );
        return;
    }

    bool rc = m_Engine->update( fullsqlcmd, naffected );
    if ( !rc && !g_AppConfig.isIgnoreErrorcode( m_Engine->errorcode() ) ) {
        // 出错了
        m_ReserveQueue->append( table, idxstr, fullsqlcmd );
    }
}

void DBThread::remove(
    const Tablename & table, const std::string & idxstr,
    const std::string & sqlcmd, const std::vector<std::string> & values )
{
    uint32_t naffected = 0;

    // fullcmd
    std::string fullsqlcmd;
    MysqlEngine::escapesqlcmd( fullsqlcmd, m_Engine, sqlcmd, values );

    // 队列有数据的情况下添加到队列中
    if ( m_ReserveQueue->isException( table, idxstr ) ) {
        m_ReserveQueue->append( table, idxstr, fullsqlcmd );
        return;
    }

    bool rc = m_Engine->remove( fullsqlcmd, naffected );
    if ( !rc && !g_AppConfig.isIgnoreErrorcode( m_Engine->errorcode() ) ) {
        m_ReserveQueue->append( table, idxstr, fullsqlcmd );
    }
}

void DBThread::replace( const Tablename & table,
    const std::string & idxstr, uint32_t transid,
    const std::string & sqlcmd, const std::vector<std::string> & values )
{
    uint64_t insertid = 0;
    uint32_t naffected = 0;

    // fullcmd
    std::string fullsqlcmd;
    MysqlEngine::escapesqlcmd( fullsqlcmd, m_Engine, sqlcmd, values );

    // 队列有数据的情况下添加到队列中
    if ( m_ReserveQueue->isException( table, idxstr ) ) {
        m_ReserveQueue->append( table, idxstr, fullsqlcmd );
        return;
    }

    bool rc = m_Engine->replace( fullsqlcmd, insertid, naffected );
    if ( !rc && !g_AppConfig.isIgnoreErrorcode( m_Engine->errorcode() ) ) {
        m_ReserveQueue->append( table, idxstr, fullsqlcmd );
    } else if ( transid != 0 ) {
        POST<AutoIncreaseTask>(
            eTaskType_AutoIncrease,
            new AutoIncreaseTask( table, transid, std::to_string( insertid ) ) );
    }
}

void DBThread::failover( const Tablename & table, const std::string & idxstr )
{
    m_ReserveQueue->failover( table, idxstr );
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

DBThreads::DBThreads( uint8_t nthreads )
    : m_Number( nthreads )
{}

DBThreads::~DBThreads()
{}

bool DBThreads::initialize()
{
    // NOTICE: 开服启动脚本已经确保数据库存在

    // 开启数据库线程
    for ( uint8_t i = 0; i < m_Number; ++i ) {
        DBThread * th = new DBThread( i );
        if ( th == nullptr ) {
            break;
        }

        // 设置线程属性
        th->setDetach();

        // 线程开始
        if ( !th->start() ) {
            break;
        }

        m_Threads.push_back( th );
    }

    if ( m_Threads.size() != m_Number ) {
        finalize();
        return false;
    }

    return true;
}

void DBThreads::finalize()
{
    for ( const auto & th : m_Threads ) {
        if ( th != nullptr ) {
            th->stop();
            delete th;
        }
    }

    m_Threads.clear();
}

bool DBThreads::check()
{
    for ( size_t i = 0; i < m_Threads.size(); ++i ) {
        if ( !m_Threads[i]->isRunning() ) {
            return false;
        }
    }

    return true;
}

bool DBThreads::post(
    const Tablename & table,
    const std::string & indexkey,
    DBMethod cmd, TransID transid,
    std::string && sqlcmd, std::vector<std::string> && values, QueryResultsHandler * handler )
{
    DBCommand * command = new DBCommand(
        table, cmd, transid, indexkey, std::move(sqlcmd), std::move(values), handler );

    // 计算DB线程索引
    uint8_t index = utils::HashFunction::djb( indexkey.c_str(), indexkey.size() ) % m_Number;

    if ( command == nullptr ) {
        return false;
    }

    if ( !m_Threads[index]->push2Database( command ) ) {
        delete command;
        return false;
    }

    return true;
}

} // namespace data
