
#include <Python.h>
#include "base/base.h"
#include "base/endpoint.h"

#include "utils/xtime.h"
#include "utils/utility.h"
#include "utils/transaction.h"

#include "io/slave.h"
#include "io/harbor.h"
#include "lua/luaenv.h"
#include "extra/ticksvc.h"
#include "extra/masterproc.h"
#include "domain/serverstats.h"

#include "task.h"
#include "config.h"
#include "startup.h"
#include "logger.h"
#include "handler.h"
#include "dbthreads.h"
#include "tablecache.h"
#include "reservequeue.h"

#include "server.h"

namespace data {

bool GameApp::initialize()
{
    if ( ReserveQueue::isException( APP_META_PATH ) ) {
        LOG_FATAL( "GameAppWrapper::initialize() : found *.SQLCMD(s) in '{}' .\n", APP_META_PATH );
        return false;
    }

    // 从binlogger中恢复数据
    if ( !recover() ) {
        return false;
    }

    // 只有0号实例才会处理启动脚本
    if ( MASTER.instid() == 0 ) {
        return runStartupScript();
    }

    return true;
}

bool GameApp::startup()
{
    if ( !Slave::instance().initialize(
             MASTER.module(), HostType::Data, MASTER.endpoint(), appthread(), &g_AppConfig ) ) {
        LOG_FATAL( "GameApp::startup() : Slave::initialize({}) failed .\n", MASTER.endpoint() );
        return false;
    }

    // 初始化数据原型
    m_ProtoType.initialize();
    ILuaEnv::addpath(
        m_ProtoType.state(), 1, g_AppConfig.getRootPath() );
    m_ProtoType.state()->dofile( g_AppConfig.getRoutineScript() );
    LOG_INFO( "GameApp::startup('{}') : TableProtoType's SchemeVersion is {} .\n", g_AppConfig.getRoutineScript(), m_ProtoType.schemeversion().c_str() );

    // 初始化缓存池
    initCaches();

    // 创建定时器/事务管理器/DB线程组
    m_BinLogger = new BinLogger();
    m_TickService = new TickService( precision() );
    m_DBThreads = new DBThreads( g_AppConfig.getDBThreadCount() );
    m_TransManager = new utils::TransactionManager( m_TickService->getTimer() );
    assert( m_BinLogger != nullptr && m_DBThreads != nullptr
        && m_TickService != nullptr && m_TransManager != nullptr );

    // 启动定时器
    if ( !m_TickService->start() ) {
        return false;
    }

    // 初始化DB线程组
    if ( !m_DBThreads->initialize() ) {
        LOG_FATAL( "GameApp::startup() : Database Threads start failed .\n" );
        return false;
    }

    // 开启IO服务
    if ( !startHarborService() ) {
        return false;
    }

    // 初始化数据库日志
    if ( !m_BinLogger->initialize() ) {
        LOG_FATAL( "GameApp::startup() : BinLogger Initialize failed .\n" );
        return false;
    }

    return true;
}

void GameApp::stopend()
{
    LOG_INFO( "GameApp::stopend() ...\n" );

    // 处理消息
    appthread()->cleanup();
    // 销毁缓存池
    finalCaches();

    // 销毁日志文件
    if ( m_BinLogger != nullptr ) {
        m_BinLogger->finalize( m_DBThreads->check() );
        delete m_BinLogger;
        m_BinLogger = nullptr;
    }

    // 消息DB线程组
    if ( m_DBThreads != nullptr ) {
        m_DBThreads->finalize();
        delete m_DBThreads;
        m_DBThreads = nullptr;
    }

    // 事务管理器
    if ( m_TransManager != nullptr ) {
        delete m_TransManager;
        m_TransManager = nullptr;
    }
    // 定时器
    if ( m_TickService != nullptr ) {
        m_TickService->stop();
        delete m_TickService;
        m_TickService = nullptr;
    }

    // 停止服务
    stopHarborService();
    m_ProtoType.finalize();

    Slave::instance().finalize();

    std::string output = STATS_INFO();
    if ( !output.empty() ) LOG_INFO( "{}\n", output );
}

void GameApp::idle()
{
    // 检查DB线程
    if ( !m_DBThreads->check() ) {
        appthread()->stoping();
    }

    // 定时器
    m_TickService->run( appthread()->now() );
}

void GameApp::process( int32_t type, void * task )
{
    m_HarborService->process( type, task );

    switch ( type ) {
        // 网络消息
        case eTaskType_SSMessage: {
            SSMessage * msg = static_cast<SSMessage *>( task );

            switch ( msg->head.type ) {
                case eMessageType_Command: {
                    int64_t s = STATS_START();
                    CommandHandler::process( msg );
                    STATS_STOP( msg->head.cmd, s );
                } break;

                case eMessageType_Transfer:
                case eMessageType_Broadcast:
                    LOG_ERROR( "GameApp::onTask(Type:{}, MessageID:{:#06x}) : this Message is INVALID .\n", msg->head.type, msg->head.cmd );
                    break;
            }

            delete msg;
        } break;

            // 数据库查询结果
        case eTaskType_Results: {
            int64_t s = STATS_START();
            ResultsTask * t = static_cast<ResultsTask *>( task );
            CommandHandler::doQueryResults( t->transid, t->results );
            delete t;
            STATS_STOP( 0xffff, s );
        } break;

            // 数据库原始结果
        case eTaskType_Datablock: {
            DatablockTask * t = static_cast<DatablockTask *>( task );
            CommandHandler::doQueryDatablock( t->transid, t->tablename, t->results );
            delete t;
        } break;

            // 数据库自增结果
        case eTaskType_AutoIncrease: {
            AutoIncreaseTask * t = static_cast<AutoIncreaseTask *>( task );
            CommandHandler::doAutoIncrease( t->transid, t->lastid );
            delete t;
        } break;

            // 错误掩码消息
        case eTaskType_Exception: {
            ExceptionTask * t = static_cast<ExceptionTask *>( task );
            CommandHandler::doDataException( t->tablename, t->indexstring );
            delete t;
        } break;
    }
}

TableCache * GameApp::getCache( const Tablename & table ) const
{
    auto result = m_DatabaseCache.find( table );
    if ( result != m_DatabaseCache.end() ) {
        return result->second;
    }
    return nullptr;
}

void GameApp::initCaches()
{
    // 创建缓存池
    for ( const auto & table : m_ProtoType.tables() ) {
        TableCache * cache = new TableCache( table, m_ProtoType.isComplete( table ) );
        assert( cache != nullptr );
        m_DatabaseCache.emplace( table, cache );
    }
}

void GameApp::finalCaches()
{
    // 清空Cache
    for ( auto & cache : m_DatabaseCache ) {
        // dbthread正在运行的情况下
        // 刷新datacache
        if ( m_DBThreads->check() ) {
            cache.second->flush2DB();
        }
        delete cache.second;
    }
}

// 恢复
bool GameApp::recover()
{
    if ( !BinLogger::isRecover( APP_META_PATH ) ) {
        return true;
    }

    MysqlEngine * engine = new MysqlEngine(
        g_AppConfig.getDBHost(), g_AppConfig.getDBPort() );
    engine->setCharsets( g_AppConfig.getDBCharsets() );
    engine->setToken( g_AppConfig.getDBUsername(), g_AppConfig.getDBPassword() );
    // 初始化
    if ( !engine->initialize() ) {
        LOG_ERROR( "GameAppWrapper::recover() : initialize the MySQLEngine(Database:'{}') failed .\n", g_AppConfig.getDatabase() );
        return false;
    }
    // 选择数据库
    engine->selectdb( g_AppConfig.getDatabase() );

    // 恢复binlog
    bool rc = BinLogger::recover( engine, APP_META_PATH );

    engine->finalize();
    delete engine;
    return rc;
}

// 启动初始化脚本
bool GameApp::runStartupScript()
{
    LOG_TRACE( "GameApp::runStartupScript('{}') ...\n", g_AppConfig.getStartupScript() );
    // 本次启动不需要维护
    if ( ::access( g_AppConfig.getStartupScript().c_str(), F_OK ) != 0 ) {
        return true;
    }

    // 处理脚本
    StartupScripts scripts;
    scripts.addpath( "." );
    scripts.addpath( "lib" );
    scripts.run( g_AppConfig.getStartupScript().c_str() );

    return true;
}

bool GameApp::startHarborService()
{
    const Endpoint * ep = g_AppConfig.getEndpoint();
    if ( ep == nullptr ) {
        LOG_FATAL( "GameApp::startup() : can't get ProxyServer config .\n" );
        return false;
    }

    m_HarborService = new HarborService(
        MASTER.module(), &g_AppConfig, appthread() );
    assert( m_HarborService != nullptr && "create HarborService failed" );

    // 启动服务
    if ( !m_HarborService->start() ) {
        LOG_FATAL( "GameApp::startup() : start AgentService failed .\n" );
        return false;
    }

    // 没有配置不同步
    if ( !g_AppConfig.getConnectList().empty()
        && !m_HarborService->connects( g_AppConfig.getConnectList() ) ) {
        return false;
    }

    // 监听端口号
    if ( !m_HarborService->listen( NetType::TCP, ep->host.c_str(), ep->port ) ) {
        LOG_FATAL( "GameApp::startup() : listen({}) failed .\n", *ep );
        return false;
    }

    LOG_INFO( "GameApp::startHarborService() : listen({}) succeed .\n", *ep );
    return true;
}

void GameApp::stopHarborService()
{
    if ( m_HarborService != nullptr ) {
        m_HarborService->halt();

        // 销毁
        m_HarborService->stop();
        delete m_HarborService;
        m_HarborService = nullptr;
    }
}

} // namespace data
