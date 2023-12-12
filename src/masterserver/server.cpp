
#include <cassert>

#include "io/task.h"
#include "io/slave.h"
#include "io/framework.h"
#include "base/base.h"

#include "http/xcurl.h"
#include "http/curlthread.h"

#include "utils/xtime.h"
#include "utils/transaction.h"

#include "extra/ticksvc.h"
#include "extra/httpsvc.h"
#include "extra/masterproc.h"

#include "message/protocol.h"

#include "config.h"
#include "handler.h"

#include "server.h"

namespace master {

bool GameApp::startup()
{
    // 是否需要连接全局服
    if ( !g_AppConfig.getGlobalEndpoint().empty() ) {
        Slave::instance().initialize(
            MASTER.module(), HostType::Master, g_AppConfig.getGlobalEndpoint(), appthread(), &g_AppConfig, 0 );
    }

    m_TickService = new TickService( precision() );
    assert( m_TickService != nullptr && "create TickService failed" );
    if ( !m_TickService->start() ) {
        return false;
    }

    m_TransactionManager = new utils::TransactionManager( m_TickService->getTimer() );

    // 初始化curl线程组
    m_CurlThreads = new CurlThreads( g_AppConfig.getApiThreads() );
    assert( m_CurlThreads != nullptr && "create CCurlThreads failed" );
    if ( !m_CurlThreads->initialize() ) {
        return false;
    }

    // 开启httpservice
    if ( !startHttpService() ) {
        return false;
    }

    // 开启服务
    if ( !startHarborService() ) {
        return false;
    }

    return true;
}

void GameApp::alter( uint32_t state )
{
    // TODO:
}

void GameApp::flush()
{
    // TODO:
}

void GameApp::reload()
{
    m_HarborService->broadcastFramework();
}

void GameApp::idle()
{
    // TODO: 如果有定时器的话, 在此处调度
    // TODO: 业务逻辑
    m_TickService->run( appthread()->now() );
}

void GameApp::process( int32_t type, void * task )
{
    m_HarborService->process( type, task );

    switch ( type ) {
        case eTaskType_HttpMessage: {
            HttpRequestTask * t = static_cast<HttpRequestTask *>( task );
            // 处理gm工具消息
            CommandHandler::process( t->sid, t->request );
            delete t;
        } break;

        case eTaskType_SSMessage: {
            SSMessage * cmd = static_cast<SSMessage *>( task );
            CommandHandler::process( cmd );
            delete cmd;
        } break;

        default:
            break;
    }
}

void GameApp::stopend()
{
    // 处理完所有的请求
    appthread()->cleanup();

    stopHttpService();
    stopHarborService();

    if ( m_TransactionManager != nullptr ) {
        delete m_TransactionManager;
        m_TransactionManager = nullptr;
    }

    if ( m_TickService != nullptr ) {
        m_TickService->stop();
        delete m_TickService;
        m_TickService = nullptr;
    }

    if ( m_CurlThreads != nullptr ) {
        m_CurlThreads->finalize();
        delete m_CurlThreads;
        m_CurlThreads = nullptr;
    }

    if ( !g_AppConfig.getGlobalEndpoint().empty() ) {
        Slave::instance().finalize();
    }
}

bool GameApp::isComposeMail()
{
    time_t t = appthread()->now( true ) / 1000;

    if ( t != m_Timestamp ) {
        m_Timestamp = t;
        m_ComposeMailCount = 0;
    }

    return ++m_ComposeMailCount < 100;
}

bool GameApp::startHarborService()
{
    // 初始化网络服务
    m_HarborService = new HarborService(
        MASTER.module(), &g_AppConfig, appthread() );
    assert( m_HarborService != nullptr && "create CHarborService failed" );

    if ( !m_HarborService->start() ) {
        LOG_FATAL( "GameApp::startHarborService() failed.\n" );
        return false;
    }

    const Endpoint * endpoint = g_AppConfig.getEndpoint();
    if ( endpoint == nullptr ) {
        LOG_FATAL( "GameApp::startHarborService() : has no info about eHostType_Master .\n" );
        return false;
    }

    // 获取admin服务器信息
    if ( !m_HarborService->listen( NetType::TCP, endpoint->host.c_str(), endpoint->port ) ) {
        LOG_FATAL( "GameApp:start() listen({}) failed .\n", *endpoint );
        return false;
    }

    LOG_INFO( "GameApp(ThreadCount:{}) : listen({}) success .\n", g_AppConfig.getFramework()->getThreadCount(), *endpoint );

    return true;
}

bool GameApp::startHttpService()
{
    m_HttpService = new HttpService( 1, 1000 );
    assert( m_HttpService != nullptr && "create HttpService failed" );

    // 初始化部分参数
    m_HttpService->setAppthread( appthread() );
    m_HttpService->setKeepaliveSeconds( 90 );

    if ( !m_HttpService->start() ) {
        return false;
    }

    if ( !m_HttpService->listen( NetType::TCP,
             g_AppConfig.getGMEndpoint().host.c_str(), g_AppConfig.getGMEndpoint().port ) ) {
        LOG_FATAL( "GameApp({}) start HttpService failed .\n", g_AppConfig.getGMEndpoint() );
        return false;
    }

    LOG_INFO( "GameApp({}) start HttpService success .\n", g_AppConfig.getGMEndpoint() );

    return true;
}

void GameApp::stopHttpService()
{
    if ( m_HttpService != nullptr ) {
        m_HttpService->halt();
        // 销毁
        m_HttpService->stop();
        delete m_HttpService;
        m_HttpService = nullptr;
    }
}

void GameApp::stopHarborService()
{
    if ( m_HarborService != nullptr ) {
        LOG_INFO( "GameApp stop ...\n" );
        m_HarborService->halt();
        // 销毁
        m_HarborService->stop();
        delete m_HarborService;
        m_HarborService = nullptr;
    }
}

} // namespace master
