
#include <cstdio>
#include <cstring>
#include <cassert>

#include <sys/time.h>
#include <sys/resource.h>
#if defined(__APPLE__)
#include <sys/param.h>
#include <sys/mount.h>
#include <malloc/malloc.h>
#else
#include <malloc.h>
#include <sys/statfs.h>
#endif

#include "base/base.h"
#include "utils/file.h"
#include "utils/xtime.h"
#include "utils/utility.h"
#include "utils/arguments.h"
#include "utils/portability.h"
#include "io/io.h"
#include "io/framework.h"

#if defined( USE_JEMALLOC )
#include <jemalloc/jemalloc.h>
#endif

#include "masterproc.h"

// 全局选项
// g_Logger             - 全局日志文件
utils::LogFile * g_Logger;

void sigint_handler( int32_t signo )
{
    MASTER.signal( SIGQUIT );
}

IApplication::IApplication( int32_t precision, int32_t taskcount )
    : m_Precision( precision ),
      m_MaxTaskCount( taskcount ),
      m_InitState( 0 )
{}

IApplication::~IApplication()
{}

void IApplication::update( uint32_t state, SwitchState st )
{
    if ( st == SwitchState::On ) {
        m_InitState |= state;
    } else {
        m_InitState ^= state;
        LOG_TRACE( "IApplication::update({:#010x}) : the Server complete this InitializeState({:#010x}).\n", m_InitState, state );

        if ( m_InitState == 0 ) {
            appthread()->setPeakCount( taskcount() );
            LOG_INFO( "IApplication::update() : the Server(MaxTaskcount:{}) complete the InitializeState, provide all SERVICE(S) Now ...\n", taskcount() );
        }
    }
}

class AppThread : public utils::IWorkThread
{
public:
    AppThread( IApplication * app )
        : utils::IWorkThread( app->precision(), app->taskcount() ),
          m_Application( app )
    {
        m_Application->m_Thread = this;
    }

    virtual ~AppThread()
    {}

    virtual bool onStart() { return m_Application->startup(); }
    virtual void onStop() { m_Application->stopend(); }
    virtual void onIdle() { m_Application->idle(); }
    virtual void onTask( int32_t type, void * task ) { m_Application->process( type, task ); }

public:
    IApplication * application() const { return m_Application; }

private:
    IApplication * m_Application;
};

MasterProcess::MasterProcess()
    : m_InstID( 0 ),
      m_Timezone( utils::TimeUtils::timezone() ),
      m_StartTime( utils::time( ) ),
      m_MasterPid( pthread_self() ),
      m_Status( RunStatus::Init ),
      m_Application( nullptr )
{}

MasterProcess::~MasterProcess()
{}

void MasterProcess::signal( int32_t signo, const ReloadArgs & args )
{
    m_Application->reloadargs( args );
    pthread_kill( m_MasterPid, signo );
}

template<> std::string MasterProcess::env( const std::string & key, const std::string & default_value ) const
{
    char * value = getenv( key.c_str() );
    if ( value != nullptr ) {
        return value;
    }
    return default_value;
}

template<> int32_t MasterProcess::env( const std::string & key, const int32_t & default_value ) const
{
    char * value = getenv( key.c_str() );
    if ( value != nullptr ) {
        return std::atoi( value );
    }
    return default_value;
}

template<> int64_t MasterProcess::env( const std::string & key, const int64_t & default_value ) const
{
    char * value = getenv( key.c_str() );
    if ( value != nullptr ) {
        return std::atoll( value );
    }
    return default_value;
}

void MasterProcess::help()
{
    fmt::print( "{} Version: {}-v{}, Timezone:{}, with {}, with {}\n",
        m_Appname, m_Module, m_AppVersion, m_Timezone, IIOService::version(), "Mutex");
    fmt::print( "Usage: {} [-vd] [-h <host>] [-p <port>] [-w <working directory>] [-i <instance>]\n\n", m_Module );
    fmt::print( "Options:\n" );
    fmt::print( "   -d                      : daemonize\n" );
    fmt::print( "   -i,--instance           : start app instance\n" );
    fmt::print( "   -h,--host               : remote MasterServer address\n" );
    fmt::print( "   -p,--port               : remote MasterServer port\n" );
    fmt::print( "   -v,--version            : show this version information\n" );
    fmt::print( "   -w,--workdir            : change work path to <workpath> ( default: the CurrentPath )\n" );
}

void MasterProcess::version()
{
    fmt::print( "{} Version: {}-v{}, Timezone:{}, with {}, with {}\n",
        m_Appname, m_Module, m_AppVersion, m_Timezone, IIOService::version(), "Mutex");
}

void MasterProcess::init( int argc, const char ** argv, const char * appname, const char * appversion )
{
    bool isdaemon = false;
    utils::Arguments args( argc, argv );

    //
    m_InstID = 0;
    m_Appname = appname;
    m_AppVersion = appversion;
    m_Module = args.command();

    if ( args.has( "?" ) ) {
        help(); exit( 0 );
    }
    if ( args.has( "-v" ) || args.has( "--version" ) ) {
        version(); exit( 0 );
    }
    if ( args.has( "-d" ) ) {
        isdaemon = true;
    }
    if ( args.has( "-h" ) || args.has( "--host" ) ) {
        auto val = args[ "-h" ];
        if ( val.empty() ) val = args[ "--host" ];
        m_Endpoint.host = val;
    }
    if ( args.has( "-p" ) || args.has( "--port" ) ) {
        auto val = args[ "-p" ];
        if ( val.empty() ) val = args[ "--port" ];
        m_Endpoint.port = std::atoi( val.c_str() );
    }
    if ( args.has( "-w" ) || args.has( "--workdir" ) ) {
        auto val = args[ "-w" ];
        if ( val.empty() ) val = args[ "--workdir" ];
        chdir( val.c_str() );
    }
    if ( args.has( "-i" ) || args.has( "--instance" ) ) {
        auto val = args[ "-i" ];
        if ( val.empty() ) val = args[ "--instance" ];
        m_InstID = std::atoi( val.c_str() );
        if ( m_InstID != 0 ) {
            m_Module += "#";
            m_Module += std::to_string( m_InstID );
        }
    }

    // 后台执行
    if ( isdaemon ) {
        daemon( 1, 0 );
    }
}

void MasterProcess::purge()
{
#if defined( USE_JEMALLOC )
    std::string tmp;
    unsigned narenas = 0;
    size_t sz = sizeof( unsigned );
    if ( !mallctl( "arenas.narenas", &narenas, &sz, nullptr, 0 ) ) {
        tmp = fmt::format( "arena.{}.purge", narenas );
        if ( !mallctl( tmp, nullptr, 0, nullptr, 0 ) ) {
            // 刷新jemalloc脏页
            LOG_INFO( "{} purging dirty pages success.\n", m_Module );
            return;
        }
    }
#else
    LOG_ERROR( "{} purging dirty pages failed (compile need -ljemalloc).\n", m_Module );
#endif
}

int MasterProcess::final()
{
    return 0;
}

int MasterProcess::run( IApplication * server, ISysConfigfile * sysconf )
{
    // 赋值
    m_Application = server;

    // 写入pid
    int32_t pidfile = utils::FileUtils::pidfopen( fmt::format( "run/{}.pid", m_Module ) );
    if ( pidfile < 0 ) {
        return -1;
    }

    // 打开全局日志文件
    g_Logger = new utils::LogFile( "log", m_Module.c_str() );
    if ( !g_Logger->open() ) {
        close( pidfile );
        return -2;
    }

    // 初始化随机种子
    srand( time( nullptr ) );
    LOG_INFO( "{}-v{} [{}] initialize ...\n", m_Module, m_AppVersion, getpid() );

    // 加载系统配置
    if ( !sysconf->load() ) {
        LOG_ERROR( "{}-v%{} [{}] load the SystemConfig('{}') failed .\n",
            m_Module, m_AppVersion, getpid(), sysconf->getConfigfile() );
        g_Logger->close();
        delete g_Logger;
        close( pidfile );
        return -2;
    }

    // 打印日志等级
    LOG_INFO( "the {}'s LogLevel : {} .\n", m_Module, sysconf->getLogLevel() );

    // 以命令行参数为准
    if ( m_Endpoint.empty() ) {
        m_Endpoint = sysconf->getMasterEndpoint();
    }

    // 创建逻辑线程
    AppThread * appthread = new AppThread( m_Application );
    assert( appthread != nullptr && "new AppThread failed" );

    // 初始化
    if ( !m_Application->initialize() ) {
        //
        sysconf->unload();
        delete appthread;
        // 销毁日志文件
        g_Logger->close();
        delete g_Logger;
        // 关闭pid文件
        close( pidfile );
        return -2;
    }

    // FIXME: Py_Finilize()会改变信号处理函数
    ::signal( SIGPIPE, SIG_IGN );
    // FIXME: gdb与sigwait()冲突
    // 具体原因: https://bugzilla.kernel.org/show_bug.cgi?id=9039
    // 所以这边只能先注册SIGINT的处理函数, 收到该信号再触发SIGQUIT
    ::signal( SIGINT, sigint_handler );

    // 初始化信号
    sigset_t waitset;
    sigemptyset( &waitset );
    sigaddset( &waitset, SIGHUP );
    sigaddset( &waitset, SIGQUIT );
    sigaddset( &waitset, SIGTERM );
    sigaddset( &waitset, SIGUSR1 );
    sigaddset( &waitset, SIGUSR2 );
    // BLOCK所有信号
    pthread_sigmask( SIG_BLOCK, &waitset, nullptr );

    // 服务开启
    LOG_INFO( "{} start ...\n", m_Appname );
    // 启动进程
    appthread->start();

    // 设置日志等级/日志文件大小
    g_Logger->setLevel( sysconf->getLogLevel() );
    g_Logger->setMaxSize( sysconf->getLogFilesize() );

    // 服务运行中 ...
    while ( appthread->isActive()
        && m_Status != RunStatus::Stop ) {
        siginfo_t info;
        struct timespec outtime;
        outtime.tv_sec = 0;
        outtime.tv_nsec = 20 * 1000 * 1000; // 20ms

        int32_t rc = sigtimedwait( &waitset, &info, &outtime );
        if ( rc != -1 ) {
            switch ( info.si_signo ) {
                case SIGINT:
                case SIGTERM:
                case SIGQUIT:
                    m_Status = RunStatus::Stop;
                    break;

                case SIGHUP: {
                    // 重新加载配置文件
                    LOG_INFO( "{} reload all configure file(s) ...\n", m_Module );

                    // 重新加载系统配置文件
                    sysconf->reload();
                    g_Logger->setLevel( sysconf->getLogLevel() );
                    g_Logger->setMaxSize( sysconf->getLogFilesize() );

                    // 服务器reload()
                    m_Application->reload();
                    // 刷新日志
                    g_Logger->flush();
                } break;

                case SIGUSR1: {
                    // 刷新服务器日志
                    m_Application->flush();
                    // 刷新系统日志
                    g_Logger->flush();
                } break;

                case SIGUSR2: {
                    // 释放jemalloc脏页
                    purge();
                } break;
            }
        }

#if defined( __DEBUG__ )
        // 测试环境每隔100ms刷新一次日志
        m_Application->flush();
        g_Logger->flush();
#endif
    }

    // 还原日志等级
    g_Logger->setLevel();

    // 服务退出
    appthread->stop();
    LOG_INFO( "{} is stopped .\n", m_Appname );
    delete appthread;
    appthread = nullptr;

    // 销毁
    // 服务特有销毁
    m_Application->finalize();
    // 卸载系统配置
    sysconf->unload();
    //
    LOG_INFO( "{}-v{} [{}] finalize .\n\n", m_Module, m_AppVersion, getpid() );
    g_Logger->close();
    delete g_Logger;

    // 关闭pidfile
    close( pidfile );

    return 0;
}

int32_t MasterProcess::diskusage() const
{
    char cwd[512];
    struct statfs fs;

    if ( statfs( getcwd( cwd, 512 ), &fs ) != 0 ) {
        return 0;
    }

    size_t ntotal = fs.f_bsize * fs.f_blocks;
    size_t navail = fs.f_bsize * fs.f_bavail;

    return 100.0f - (double)navail / (double)ntotal * 100.0f;
}

int64_t MasterProcess::uptime() const
{
    return utils::time() - m_StartTime;
}

double MasterProcess::cputime() const
{
    struct rusage usage;
    getrusage( RUSAGE_SELF, &usage );

    double sec = usage.ru_utime.tv_sec + usage.ru_stime.tv_sec;
    double usec = usage.ru_utime.tv_usec + usage.ru_stime.tv_usec;

    return sec + usec / 1000000.0f;
}

double MasterProcess::cpuutilization() const
{
    return cputime() / uptime();
}

int64_t MasterProcess::getUseMemory() const
{
    int64_t size = 0;
#if defined( __linux__ )
    struct mallinfo stats = mallinfo();
    size = stats.uordblks + stats.hblkhd;
#elif defined( __APPLE__ )
    size = malloc_size( nullptr );
#endif
    return size;
}
