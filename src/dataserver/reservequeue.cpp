
#include <cstdio>

#include "base/base.h"
#include "base/limits.h"
#include "utils/file.h"
#include "utils/xtime.h"
#include "utils/utility.h"
#include "mysql/engine.h"
#include "extra/masterproc.h"

#include "task.h"
#include "config.h"
#include "server.h"
#include "reservequeue.h"

namespace data {

inline std::string KEYJOIN( const std::string & table, const std::string & key = "" )
{
    std::string k = table;

    k += "#";
    k += key;

    return k;
}

ReserveQueue::ReserveQueue( uint8_t index )
    : m_Index( index ),
      m_ErrorLogger( nullptr )
{}

ReserveQueue::~ReserveQueue()
{
    for ( const auto & cl : m_CmdQueue ) {
        getErrorLog()->append( cl.sqlcmd + ";\n" );
    }

    if ( m_ErrorLogger != nullptr ) {
        m_ErrorLogger->sync();
        m_ErrorLogger->close();
        delete m_ErrorLogger;
        m_ErrorLogger = nullptr;
    }

    for ( const auto & error : m_ExceptionTable ) {
        LOG_TRACE( "ReserveQueue sync '{}':{}:{} SQLCMD(s) to SQLCMDFILE ...\n",
            error.first, error.second.count, error.second.timestamp );
    }
    m_CmdQueue.clear();
    m_ExceptionTable.clear();
}

bool ReserveQueue::isException( const char * path )
{
    std::vector<std::string> logfiles;

    utils::PathUtils( path ).traverse(
        logfiles, ReserveQueue::suffix().c_str() );

    // 删除空文件
    for ( auto it = logfiles.begin(); it != logfiles.end(); ) {
        if ( 0 < utils::FileUtils::getFileSize( *it ) ) {
            ++it;
        } else {
            ::remove( it->c_str() );
            it = logfiles.erase( it );
        }
    }

    return !logfiles.empty();
}

bool ReserveQueue::flush2DB( MysqlEngine * engine )
{
    for ( ; !empty(); ) {
        bool rc = true;
        time_t timestamp = m_CmdQueue.front().timestamp;
        std::string & sqlcmd = m_CmdQueue.front().sqlcmd;

        // 刷新到数据库中
        rc = engine->execute( sqlcmd );
        if ( !rc ) {
            // 没有超时的情况下
            if ( g_AppConfig.getFlushInterval()
                >= utils::time() - timestamp ) {
                return true;
            }

            return save2Logger();
        }

        // 成功后移除
        std::string k = KEYJOIN(
            m_CmdQueue.front().table, m_CmdQueue.front().indexstring );
        auto result = m_ExceptionTable.find( k );
        if ( result != m_ExceptionTable.end() ) {
            if ( --result->second.count == 0 ) {
                m_ExceptionTable.erase( result );
            }
        }
        // 移除
        m_CmdQueue.pop_front();
    }

    return true;
}

bool ReserveQueue::isException( const Tablename & table, const std::string & idxstr ) const
{
    return m_ExceptionTable.end() != m_ExceptionTable.find( KEYJOIN( table, idxstr ) );
}

void ReserveQueue::failover( const Tablename & table, const std::string & idxstr )
{
    auto result = m_ExceptionTable.find( KEYJOIN( table, idxstr ) );
    if ( result != m_ExceptionTable.end() ) {
        LOG_WARN( "ReserveQueue::failover() : failover this Exception(Table:'{}', IndexString:'{}', Count:{}, Timestamp:'{}') .\n",
            table, idxstr, result->second.count, result->second.timestamp );
        m_ExceptionTable.erase( result );
    }
}

void ReserveQueue::append( const Tablename & table, const std::string & idxstr, const std::string & sqlcmd )
{
    uint32_t method = 0; // 0:写内存;1:写文件
    std::string k = KEYJOIN( table, idxstr );

    LOG_WARN( "ReserveQueue::append() : catch this Exception(Table:'{}', IndexString:'{}') .\n", table, idxstr );

    auto result = m_ExceptionTable.find( k );
    if ( result == m_ExceptionTable.end() ) {
        m_ExceptionTable.emplace( k, DataException( 1 ) );
    } else {
        if ( result->second.count == 0 ) {
            method = 1;
        } else {
            ++result->second.count;
        }
    }

    if ( method != 0 ) {
        // 写入错误日志文件
        getErrorLog()->append( sqlcmd + ";\n" );
        getErrorLog()->flush();
    } else {
        // 指令队列
        m_CmdQueue.push_back(
            SQLCmdLine( utils::time(), sqlcmd, table, idxstr ) );
    }
}

bool ReserveQueue::save2Logger()
{
    Tablename table = m_CmdQueue.front().table;
    std::string idxstr = m_CmdQueue.front().indexstring;

    // 提交到主线程
    POST<ExceptionTask>(
        eTaskType_Exception,
        new ExceptionTask( table, idxstr ) );

    // 提取出所有错误SQL到文件中
    for ( auto it = m_CmdQueue.begin(); it != m_CmdQueue.end(); ) {
        if ( it->table != table
            || it->indexstring != idxstr ) {
            ++it;
        } else {
            // 写入错误日志文件
            getErrorLog()->append( it->sqlcmd + ";\n" );
            it = m_CmdQueue.erase( it );
        }
    }
    // 强制落地
    getErrorLog()->flush();

    time_t now = utils::time();
    std::string key = KEYJOIN( table, idxstr );
    auto result = m_ExceptionTable.find( key );
    if ( result != m_ExceptionTable.end() ) {
        result->second.count = 0;
        result->second.timestamp = now;
    } else {
        m_ExceptionTable.emplace( key, DataException( 0, now ) );
    }

    return true;
}

std::string ReserveQueue::suffix()
{
    char suffix[64];

    if ( MASTER.instid() == 0 ) {
        strncpy( suffix, "SQLCMD", 63 );
    } else {
        snprintf( suffix, 63, "%u.SQLCMD", MASTER.instid() );
    }

    return suffix;
}

std::string ReserveQueue::filename()
{
    char path[1024];

    if ( MASTER.instid() == 0 ) {
        snprintf( path, 1023, "%s/dbexception%u.SQLCMD", APP_META_PATH, m_Index );
    } else {
        snprintf( path, 1023, "%s/dbexception%u.%u.SQLCMD", APP_META_PATH, m_Index, MASTER.instid() );
    }

    return path;
}

utils::WritableFile * ReserveQueue::getErrorLog()
{
    if ( m_ErrorLogger == nullptr ) {
        m_ErrorLogger = new utils::WritableFile( filename() );
        assert( m_ErrorLogger != nullptr && "create utils::WritableFile failed" );
    }

    return m_ErrorLogger;
}

} // namespace data
