
#include <cstdio>
#include <cstdlib>
#include <cassert>
#include <cstring>

#include <time.h>
#include <unistd.h>
#include <stdarg.h>

#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pthread.h>

#if defined( __linux__ )
#include <linux/limits.h>
#else
#include <limits.h>
#endif

#include "ipcs.h"
#include "file.h"
#include "thread.h"
#include "types.h"
#include "base/support.h"
#include "fixedstring.h"

namespace utils {

//
// Thread Local Storage
//
struct ThreadData
{
    char threadid[32];
    int64_t timestamp;
    struct tm timestruct;
    int32_t fmtlen;
    char timeformat[32];

    const char * tid()
    {
        if ( threadid[0] == 0 ) {
            snprintf( threadid, 31, "%07u", gettid() );
        }

        return threadid;
    }

    const char * format()
    {
        if ( timeformat[0] == 0 ) {
            fmtlen = snprintf(
                timeformat, 31, "%04d-%02d-%02d %02d:%02d:%02d",
                timestruct.tm_year + 1900, timestruct.tm_mon + 1,
                timestruct.tm_mday, timestruct.tm_hour, timestruct.tm_min, timestruct.tm_sec );
        }

        return timeformat;
    }

    int32_t size()
    {
        if ( fmtlen == 0 ) {
            fmtlen = snprintf(
                timeformat, 31, "%04d-%02d-%02d %02d:%02d:%02d",
                timestruct.tm_year + 1900, timestruct.tm_mon + 1,
                timestruct.tm_mday, timestruct.tm_hour, timestruct.tm_min, timestruct.tm_sec );
        }

        return fmtlen;
    }

    int32_t date() const
    {
        return ( timestruct.tm_year + 1900 ) * 10000 + ( timestruct.tm_mon + 1 ) * 100 + timestruct.tm_mday;
    }
};

typedef FixedString<8192> LogLine;
__thread ThreadData t_ThreadData;

//
// 日志缓存
//
struct Buffer
{
    int32_t date = 0;     // 记录日志的日期
    int32_t index = 0;    // 记录当前日期下日志文件的索引
    uint8_t minlevel = 0; //
    size_t sizelimit = 0; // 文件大小限制
    int32_t offsets = 0;  // 日志缓存的偏移量
    char buffer[0];       // 日志缓存
};

int64_t milliseconds()
{
    int64_t now = 0;
    struct timeval tv;

    if ( ::gettimeofday( &tv, nullptr ) == 0 ) {
        now = tv.tv_sec * 1000 + tv.tv_usec / 1000;
    }

    time_t now_seconds = now / 1000;

    if ( now_seconds != t_ThreadData.timestamp ) {
        t_ThreadData.fmtlen = 0;
        t_ThreadData.timeformat[0] = 0;
        t_ThreadData.timestamp = now_seconds;
        localtime_r( &now_seconds, &t_ThreadData.timestruct );
    }

    return now;
}

void fmtprefix( LogLine & head, const char * format, uint8_t level, int64_t now, const Sourcefile & file )
{
    const char * LEVEL_DESC[] = {
        "FATAL", "ERROR", "WARN", "INFO", "TRACE", "DEBUG"
    };

    for ( ; *format; ++format ) {
        if ( *format != '%' ) {
            head.append( *format );
            continue;
        }

        switch ( *++format ) {
            case '\0':
                --format;
                break;

            case 'l':
            case 'L':
                head.append( LEVEL_DESC[level - 1] );
                break;

            case 'p':
            case 'P':
                head.append( t_ThreadData.tid() );
                break;

            case 'f':
            case 'F':
                if ( !file.empty() ) {
                    head.append( file.data, file.size );
                    head.append( ":%u", (uint32_t)file.line );
                }
                break;

            case 't':
                head.append( t_ThreadData.format(), t_ThreadData.size() );
                break;

            case 'T':
                // YYYY-mm-dd HH:MM:SS
                head.append(
                    t_ThreadData.format(), t_ThreadData.size() );
                // 毫秒数
                head.append( ".%03lu", now % 1000 );
                break;
        }
    }
}

//
// 日志记录器
//
class Logger
{
public:
    Logger( LogFile * file );
    ~Logger();
    static size_t getBlockSize();

public:
    // 初始化
    bool initialize();
    // 获取日志文件大小
    ssize_t getFileSize();
    // 获取日志文件路径
    void getLogPath( char * path );
    void getIndexPath( char * path );
    // 设置/获取今天的日期
    void setDate( int32_t today );
    int32_t getDate() const { return m_Buffer->date; }
    // 获取/设置日志等级
    void setLevel( uint8_t level );
    uint8_t getLevel() const { return m_Buffer->minlevel; }
    // 获取偏移量
    int32_t getOffset() const { return m_Buffer->offsets; }
    // 设置日志文件限制
    void setSizeLimit( size_t size ) { m_Buffer->sizelimit = size; }

public:
    bool open();
    void flush( bool sync = false );
    void append( const std::string & logline );
    void attach();
    void rotate();
    void skipday( int32_t today );

private:
    LogFile * m_LogFile;

    int32_t m_Fd;
    int32_t m_Date; // fd关联的日期
    size_t m_Size;
    Buffer * m_Buffer;
};

Logger::Logger( LogFile * file )
    : m_LogFile( file ),
      m_Fd( -1 ),
      m_Date( 0 ),
      m_Size( 0 ),
      m_Buffer( nullptr )
{}

Logger::~Logger()
{
    // 刷新日志到文件中
    flush( true );

    // 关闭文件
    if ( m_Fd != -1 ) {
        ::close( m_Fd );
        m_Fd = -1;
    }

    if ( m_Buffer != nullptr ) {
        m_LogFile->getBlock()->unlink( m_Buffer );
        m_Buffer = nullptr;
    }
}

bool Logger::initialize()
{
    // 连接内存块
    m_Buffer = (Buffer *)m_LogFile->getBlock()->link();
    assert( m_Buffer != nullptr && "Logger link ShareMemory failed" );

    // 新建内存块的时候才会初始化
    if ( m_LogFile->getBlock()->isOwner() ) {
        // 求时间
        milliseconds();
        // 初始化其他数据
        m_Buffer->offsets = 0;
        m_Buffer->sizelimit = 0;
        m_Buffer->minlevel = 0;
        m_Buffer->index = 1;
        m_Buffer->date = t_ThreadData.date();
    }

    // 计算文件块大小
    m_Size = Logger::getBlockSize();

    // 是否需要打开文件
    return open();
}

size_t Logger::getBlockSize()
{
    // 获取块大小
    struct stat fs;
    stat( "/dev/null", &fs );
    return fs.st_blksize;
}

ssize_t Logger::getFileSize()
{
    char path[PATH_MAX];
    getLogPath( path );
    return FileUtils::getFileSize( path );
}

void Logger::getLogPath( char * path )
{
    std::snprintf( path, PATH_MAX, "%s/%s-%d.log",
            m_LogFile->getPath(), m_LogFile->getModule(), m_Buffer->date );
}

void Logger::getIndexPath( char * path )
{
    std::snprintf( path, PATH_MAX, "%s/%s-%d.%d.log",
            m_LogFile->getPath(), m_LogFile->getModule(), m_Buffer->date, m_Buffer->index );
}

void Logger::setDate( int32_t today )
{
    m_Buffer->index = 1;
    m_Buffer->date = today;
}

void Logger::setLevel( uint8_t level )
{
    if ( level <= LogFile::eLogLevel_Debug ) {
        m_Buffer->minlevel = level;
    }
}

bool Logger::open()
{
    char path[PATH_MAX];

    // 拼接文件名
    getLogPath( path );

    // 关联日志
    m_Date = getDate();

    // 打开文件
    m_Fd = ::open( path, O_RDWR | O_APPEND | O_CREAT, 0644 );
    assert( m_Fd != -1 && "Logger::open() LogFile failed ." );

    return m_Fd != -1;
}

void Logger::append( const std::string & logline )
{
    size_t size = logline.size();
    char * data = const_cast<char *>( logline.c_str() );

    // 绑定日志描述符
    // 确保日志刷新到当前文件中
    attach();

    // 需要将日志数据落地到文件中
    // 日志缓冲区空间不足的情况
    if ( m_Buffer->offsets + size >= m_Size ) {
        flush();
    }

    if ( size >= m_Size ) {
        // 太长的日志行不进行缓冲直接写日志文件
        // NOTICE: 让操作系统自己去刷新硬盘缓存
        ::write( m_Fd, data, size );
    } else {
        // 记录新的日志信息
        std::memcpy( m_Buffer->buffer + m_Buffer->offsets, data, size );
        m_Buffer->offsets += size;
    }
}

void Logger::flush( bool sync )
{
    if ( m_Buffer->offsets == 0 ) {
        // 日志已经全部刷新到日志文件中
        return;
    }

    // 为了防止日志文件不完整,
    // 每次flush都会把缓冲区的数据刷新到当前日志文件中

    // 日志文件正常, 记录日志文件的尺寸
    ssize_t filesize = getFileSize();
    if ( filesize == -1 ) {
        // 文件不存在
        ::close( m_Fd );
        open();
        filesize = 0;
    }

    filesize += m_Buffer->offsets;

    // 把日志缓冲区的日志落地到已经存在的日志文件中去
    ::write( m_Fd, m_Buffer->buffer, m_Buffer->offsets );

    // 是否需要刷新到文件中
    // NOTICE: fsync()调用需要10ms左右, 所以不要经常调用
    if ( sync ) {
        ::fsync( m_Fd );
    }

    m_Buffer->offsets = 0;

    // 只有flush的情况下, 才会引发文件大小的改变
    // 此时去进行日志尺寸的检查最为合适了
    if ( m_Buffer->sizelimit != 0
        && filesize > (ssize_t)m_Buffer->sizelimit ) {
        // 当前的日志文件已经被删除或者文件尺寸过大
        rotate();
    }
}

void Logger::attach()
{
    // 判断fd与日期是否匹配
    if ( m_Date == m_Buffer->date ) {
        return;
    }

    // 已经不匹配了，解绑后再次绑定
    ::close( m_Fd );
    open();
}

void Logger::rotate()
{
    char oldfile[PATH_MAX];
    char newfile[PATH_MAX];

    getLogPath( oldfile );
    getIndexPath( newfile );

    // 关闭文件
    ::fsync( m_Fd );
    ::close( m_Fd );

    // 重命名文件, 以及增加文件索引号
    ++m_Buffer->index;
    std::rename( oldfile, newfile );

    // 打开文件
    open();
}

void Logger::skipday( int32_t today )
{
    // 刷新日志
    flush( true );

    // 关闭文件
    if ( m_Fd != -1 ) {
        ::close( m_Fd );
        m_Fd = -1;
    }

    // 设置今天的日期
    setDate( today );

    // 重新打开日志
    open();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

//
// 日志中共享内存的锁
//
// 主要解决进程间同步和线程间同步
// 可用解决方案就是两种
//      1. 记录锁
//      2. 信号量
//
// LogFile默认采用的是第二种方案
// 并基于POSIX_THREAD_PROCESS_SHARED做了优化
//
//
class LoggerLock
{
public:
    LoggerLock( const char * name )
        : m_Key( name ),
          m_Mem( nullptr ),
          m_Lock( nullptr )
    {}

    ~LoggerLock()
    {}

    bool init()
    {
#if !defined _POSIX_THREAD_PROCESS_SHARED
        m_Lock = new CSemlock( m_Key.c_str() );
        assert( m_Lock != nullptr && "create CSemlock failed" );

        bool rc = m_Lock->init();
        if ( !rc ) {
            return false;
        }
#else
        m_Mem = new CShmem( m_Key.c_str() );
        assert( m_Mem != nullptr && "create CShmem failed" );

        bool rc = m_Mem->alloc( sizeof( pthread_mutex_t ) );
        if ( !rc ) {
            return false;
        }
        m_Lock = (pthread_mutex_t *)m_Mem->link();

        pthread_mutexattr_t attr;
        pthread_mutexattr_init( &attr );
        pthread_mutexattr_setpshared( &attr, PTHREAD_PROCESS_SHARED );
        pthread_mutex_init( m_Lock, &attr );
        pthread_mutexattr_destroy( &attr );
#endif
        return true;
    }

    void final()
    {
#if !defined _POSIX_THREAD_PROCESS_SHARED
        if ( m_Lock != nullptr ) {
            delete m_Lock;
            m_Lock = nullptr;
        }
#else
        pthread_mutex_destroy( m_Lock );
        if ( m_Mem != nullptr ) {
            m_Mem->unlink( m_Lock );
            delete m_Mem;
        }
#endif
        m_Mem = nullptr;
        m_Lock = nullptr;
    }

#if !defined _POSIX_THREAD_PROCESS_SHARED
    void lock() { m_Lock->lock(); }
    void unlock() { m_Lock->unlock(); }
#else
    void lock() { pthread_mutex_lock( m_Lock ); }
    void unlock() { pthread_mutex_unlock( m_Lock ); }
#endif

private:
    std::string m_Key;
    CShmem * m_Mem;
#if !defined _POSIX_THREAD_PROCESS_SHARED
    CSemlock * m_Lock;
#else
    pthread_mutex_t * m_Lock;
#endif
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

LogFile::LogFile( const char * path, const char * module )
    : m_Path( path ),
      m_Module( module ),
      m_Block( nullptr ),
      m_Lock( nullptr ),
      m_Logger( nullptr )
{}

LogFile::~LogFile()
{}

bool LogFile::open()
{
    // 初始化锁和共享内存的key文件
    char lock_keyfile[PATH_MAX] = { 0 };
    char shmem_keyfile[PATH_MAX] = { 0 };

    // 确保文件存在
    ensure_keyfiles_exist( lock_keyfile, shmem_keyfile );

    // 初始化缓冲区锁
    m_Lock = new LoggerLock( lock_keyfile );
    if ( !m_Lock->init() ) {
        return false;
    }

    // 初始化日志缓冲区
    m_Block = new CShmem( shmem_keyfile );
    if ( !m_Block->alloc( Logger::getBlockSize() + sizeof( Buffer ) ) ) {
        // 日志缓冲区分配失败
        m_Lock->final();
        delete m_Block;
        delete m_Lock;
        return false;
    }

    m_Lock->lock();

    // 初始化日志文件
    m_Logger = new Logger( this );
    assert( m_Logger != nullptr && "create Logger failed" );

    // 初始化
    if ( !m_Logger->initialize() ) {
        m_Lock->unlock();
        m_Lock->final();
        delete m_Block;
        delete m_Lock;
        return false;
    }
    // 刷新日志
    m_Logger->flush( true );

    m_Lock->unlock();

    return true;
}

void LogFile::print( uint8_t level, const std::string & content )
{
    // 过滤非法日志等级
    assert( level <= eLogLevel_Debug );

    // 日志等级的判断
    if ( m_Logger->getLevel() != 0
        && level > m_Logger->getLevel() ) {
        // 不加锁对最低的日志等级的判断,
        // 有可能引发的顺序问题可以不做考虑
        // 最坏的情况就是
        // 改变日志等级一瞬间的部分日志信息没有记录下来, 这很严重吗?
        return;
    }

    // 时间
    milliseconds();

    // 写日志
    append( level, t_ThreadData.date(), content );
}

void LogFile::print( uint8_t level, const char * format, ... )
{
    // 过滤非法日志等级
    assert( level <= eLogLevel_Debug );

    // 日志等级的判断
    if ( m_Logger->getLevel() != 0
        && level > m_Logger->getLevel() ) {
        // 不加锁对最低的日志等级的判断,
        // 有可能引发的顺序问题可以不做考虑
        // 最坏的情况就是
        // 改变日志等级一瞬间的部分日志信息没有记录下来, 这很严重吗?
        return;
    }

    // 时间
    milliseconds();

    // 内容
    char content[8192];
    int32_t length = 0;
    va_list args;
    va_start( args, format );
    length = vsnprintf(
        content, sizeof( content ) - 1, format, args );
    va_end( args );

    if ( likely( length > 0 && (size_t)length < sizeof( content ) ) ) {
        // 写日志
        append( level, t_ThreadData.date(), std::string( content, length ) );
    } else {
        // LogLine不够用的情况下vasprintf()
        char * body = nullptr;

        va_start( args, format );
        length = vasprintf( &body, format, args );
        va_end( args );
        // 写日志
        append( level, t_ThreadData.date(), std::string( body, length ) );

        // 销毁堆中分配的行
        std::free( body );
    }
}

void LogFile::printp( uint8_t level, const char * prefix, const std::string & content )
{
    return printp( level, prefix, Sourcefile(), content );
}

void LogFile::printp( uint8_t level, const char * prefix, const Sourcefile & file, const std::string & content )
{
    // 过滤非法日志等级
    assert( level <= eLogLevel_Debug );

    // 日志等级的判断
    if ( m_Logger->getLevel() != 0
        && level > m_Logger->getLevel() ) {
        // 不加锁对最低的日志等级的判断,
        // 有可能引发的顺序问题可以不做考虑
        // 最坏的情况就是
        // 改变日志等级一瞬间的部分日志信息没有记录下来, 这很严重吗?
        return;
    }

    // 时间
    int64_t now = milliseconds();

    // head
    LogLine logprefix;
    fmtprefix( logprefix, prefix, level, now, file );

    // 写日志
    append( level, t_ThreadData.date(), std::string( logprefix.data(), logprefix.size() ) + content );
}

void LogFile::printp( uint8_t level, const char * prefix, const char * format, ... )
{
    // 过滤非法日志等级
    assert( level <= eLogLevel_Debug );

    // 日志等级的判断
    if ( m_Logger->getLevel() != 0
        && level > m_Logger->getLevel() ) {
        // 不加锁对最低的日志等级的判断,
        // 有可能引发的顺序问题可以不做考虑
        // 最坏的情况就是
        // 改变日志等级一瞬间的部分日志信息没有记录下来, 这很严重吗?
        return;
    }

    // 时间
    int64_t now = milliseconds();

    // head
    LogLine logprefix;
    fmtprefix( logprefix, prefix, level, now, Sourcefile() );

    // body
    va_list args;
    int32_t nbody = 0;
    va_start( args, format );
    nbody = vsnprintf(
        const_cast<char *>( logprefix.avail() ), logprefix.left() - 1, format, args );
    va_end( args );

    if ( likely( nbody > 0 && (size_t)nbody < logprefix.left() ) ) {
        // 写日志
        append( level, t_ThreadData.date(), std::string( logprefix.data(), logprefix.size() + nbody ) );
    } else {
        // LogLine不够用的情况下vasprintf()
        char * body = nullptr;

        va_start( args, format );
        nbody = vasprintf( &body, format, args );
        va_end( args );
        // 写日志
        append( level, t_ThreadData.date(), std::string( logprefix.data(), logprefix.size() ) + std::string( body, nbody ) );

        // 销毁堆中分配的行
        std::free( body );
    }
}

void LogFile::printp( uint8_t level, const char * prefix, const Sourcefile & file, const char * format, ... )
{
    // 过滤非法日志等级
    assert( level <= eLogLevel_Debug );

    // 日志等级的判断
    if ( m_Logger->getLevel() != 0
        && level > m_Logger->getLevel() ) {
        // 不加锁对最低的日志等级的判断,
        // 有可能引发的顺序问题可以不做考虑
        // 最坏的情况就是
        // 改变日志等级一瞬间的部分日志信息没有记录下来, 这很严重吗?
        return;
    }

    // 时间
    int64_t now = milliseconds();

    // head
    LogLine logprefix;
    fmtprefix( logprefix, prefix, level, now, file );

    // body
    va_list args;
    int32_t nbody = 0;
    va_start( args, format );
    nbody = vsnprintf(
        const_cast<char *>( logprefix.avail() ), logprefix.left() - 1, format, args );
    va_end( args );

    if ( likely( nbody > 0 && (size_t)nbody < logprefix.left() ) ) {
        // 写日志
        append( level, t_ThreadData.date(), std::string( logprefix.data(), logprefix.size() + nbody ) );
    } else {
        // LogLine不够用的情况下vasprintf()
        char * body = nullptr;

        va_start( args, format );
        nbody = vasprintf( &body, format, args );
        va_end( args );
        // 写日志
        append( level, t_ThreadData.date(), std::string( logprefix.data(), logprefix.size() ) + std::string( body, nbody ) );

        // 销毁堆中分配的行
        std::free( body );
    }
}

void LogFile::flush()
{
    m_Lock->lock();
    m_Logger->flush( true );
    m_Lock->unlock();
}

void LogFile::setLevel( uint8_t level )
{
    m_Lock->lock();
    m_Logger->setLevel( level );
    m_Lock->unlock();
}

void LogFile::setMaxSize( size_t size )
{
    m_Lock->lock();
    m_Logger->setSizeLimit( size );
    m_Lock->unlock();
}

void LogFile::close()
{
    if ( m_Lock != nullptr ) {
        m_Lock->lock();
    }

    // 强制刷新日志
    m_Logger->flush( true );

    // 销毁日志
    if ( m_Logger != nullptr ) {
        delete m_Logger;
        m_Logger = nullptr;
    }

    if ( m_Block != nullptr ) {
        // 销毁内存块
        // m_Block->free();
        delete m_Block;
        m_Block = nullptr;
    }

    if ( m_Lock != nullptr ) {
        // 销毁锁
        m_Lock->unlock();
        m_Lock->final();
        delete m_Lock;
        m_Lock = nullptr;
    }
}

void LogFile::append( uint8_t level, int32_t today, const std::string & body )
{
    // 加锁写日志
    m_Lock->lock();

    // 日志文件是否隔天了
    if ( today != m_Logger->getDate() ) {
        // 日志隔天
        m_Logger->skipday( today );
    }

    // 写日志
    // 轮换日志文件在append()中发生
    m_Logger->append( body );

    // 如果是FATAL直接刷新日志
    if ( level == eLogLevel_Fatal ) {
        m_Logger->flush();
    }

    // 解锁
    m_Lock->unlock();
}

void LogFile::ensure_keyfiles_exist( char * file1, char * file2 )
{
    std::snprintf( file1, PATH_MAX, "%s/.log_%s_lock.key", m_Path.c_str(), m_Module.c_str() );
    std::snprintf( file2, PATH_MAX, "%s/.log_%s_shmem.key", m_Path.c_str(), m_Module.c_str() );

    // 确保文件存在
    ::close( ::open( file1, O_WRONLY | O_CREAT, 0644 ) );
    ::close( ::open( file2, O_WRONLY | O_CREAT, 0644 ) );
}

} // namespace utils
