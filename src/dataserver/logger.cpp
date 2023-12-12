
#include <algorithm>
#include <cinttypes>

#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>

#include "scheme/sqlbind.h"
#include "message/dbprotocol.h"

#include "base/base.h"
#include "utils/file.h"
#include "utils/utility.h"
#include "utils/xtime.h"
#include "extra/masterproc.h"

#include "config.h"
#include "dbthreads.h"
#include "tablecache.h"
#include "mysql/result.h"

#include "logger.h"

namespace data {

template<typename T>
void SYNCFILECOMMAND( const std::string & path )
{
    HostID id = g_AppConfig.getJchHostID( HostType::Logger, g_AppConfig.getZoneID() );
    if ( id != 0 ) {
        T cmd;
        cmd.zoneid = g_AppConfig.getZoneID();
        cmd.filename = path.substr( path.find_last_of( '/' ) + 1 );
        g_HarborService->sendByHostID( id, &cmd );
    }
}

static void LOAD_CURRENT_FILE( std::string & path, size_t & offset1, size_t & offset2 )
{
    offset1 = 0;
    offset2 = 0;

    std::string content;
    utils::FileUtils::getcontent(
        BinLogger::currentfile(), content );

    int32_t index = 0;
    char *word, *brkt;
    const char * sep = ":";
    char * belist = strdup( content.c_str() );

    for ( word = strtok_r( belist, sep, &brkt );
          word;
          word = strtok_r( nullptr, sep, &brkt ) ) {
        switch ( index++ ) {
            case 0: path = word; break;
            case 1: offset1 = std::atoll( word ); break;
            case 2: offset2 = std::atoll( word ); break;
        }
    }

    std::free( belist );
}

static void STORE_CURRENT_FILE( const char * path, size_t offset1, size_t offset2 )
{
    char content[1024];
    snprintf( content, 1023, "%s:%lu:%lu", path, offset1, offset2 );
    utils::FileUtils::writelocked( BinLogger::currentfile(), content );
}

static bool RECOVER_FROM_FILE( MysqlEngine * engine, TableProtoType * prototype, const char * path, size_t offset1, size_t offset2 )
{
    int32_t fd = 0;
    bool rc = true;
    void * filecontent = nullptr;
    size_t count = 0, nloop = 0, filesize = 0;

    fd = open( path, O_RDONLY );
    if ( fd < 0 ) {
        LOG_FATAL( "RECOVER_FROM_FILE('{}') : open() failed .\n", path );
        return false;
    }

    // 文件内容以及文件尺寸
    filesize = lseek( fd, 0, SEEK_END );
    if ( filesize == 0 ) {
        close( fd );
        return true;
    }
    filecontent = mmap( 0, filesize, PROT_READ, MAP_SHARED, fd, 0 );
    if ( filecontent == MAP_FAILED ) {
        LOG_FATAL( "RECOVER_FROM_FILE('{}') : mmap() failed .\n", path );
        close( fd );
        return false;
    }

    StreamBuf unpack( (const char *)filecontent, filesize );
    while ( unpack.size() < unpack.length() ) {
        ISQLData * data = nullptr;

        Slice dbdata;
        std::string script;
        Tablename tablename;
        int64_t timestamp = 0;
        bool isreplace = false;
        DBMethod method = DBMethod::None;

        unpack.decode( timestamp );
        unpack.decode( method );
        unpack.decode( tablename );
        unpack.decode( dbdata );
        unpack.decode( script );
        unpack.decode( isreplace );

        // 跳过已经处理完成的BINLOG
        if ( ++count < offset1 ) {
            continue;
        }

        data = prototype->data( tablename );
        assert( data != nullptr && "Can't Found ProtoType" );

        // 解包
        data->decode( dbdata, eCodec_Dirty );

        // 拼接SQL
        switch ( method ) {
            case DBMethod::Insert: {
                uint64_t insertid = 0;
                std::string sqlcmd, fullsqlcmd;
                std::vector<std::string> escapevalues;

                data->insert( sqlcmd, escapevalues );
                MysqlEngine::escapesqlcmd( fullsqlcmd, engine, sqlcmd, escapevalues );
                rc = engine->insert( fullsqlcmd, insertid );
                if ( !rc && engine->errorcode() == 1062 ) {
                    rc = true;
                    LOG_TRACE( "RECOVER_FROM_FILE('{}') : skip the BinLogger's Line({}) .\n", path, count );
                }
            } break;

            case DBMethod::Update: {
                uint32_t naffected = 0;
                std::string sqlcmd, fullsqlcmd;
                std::vector<std::string> escapevalues;

                data->update( sqlcmd, escapevalues );
                MysqlEngine::escapesqlcmd( fullsqlcmd, engine, sqlcmd, escapevalues );
                rc = engine->update( fullsqlcmd, naffected );
            } break;

            case DBMethod::Remove: {
                uint32_t naffected = 0;
                std::string sqlcmd, fullsqlcmd;
                std::vector<std::string> escapevalues;

                data->remove( sqlcmd, escapevalues );
                MysqlEngine::escapesqlcmd( fullsqlcmd, engine, sqlcmd, escapevalues );
                rc = engine->remove( fullsqlcmd, naffected );
            } break;

            case DBMethod::Replace: {
                uint64_t insertid = 0;
                uint32_t naffected = 0;
                std::string sqlcmd, fullsqlcmd;
                std::vector<std::string> escapevalues;

                data->replace( sqlcmd, escapevalues );
                MysqlEngine::escapesqlcmd( fullsqlcmd, engine, sqlcmd, escapevalues );
                rc = engine->replace( fullsqlcmd, insertid, naffected );
            } break;

            case DBMethod::Invoke: {
                std::string sqlcmd, fullsqlcmd;
                std::vector<std::string> escapevalues;
                MysqlResults * results = new MysqlResults;

                nloop = 0;
                data->query( sqlcmd, escapevalues );
                MysqlEngine::escapesqlcmd( fullsqlcmd, engine, sqlcmd, escapevalues );
                engine->query( fullsqlcmd, results );

                if ( results->size() == 0 ) {
                    if ( isreplace ) {
                        uint64_t insertid = 0;
                        data->insert( sqlcmd, escapevalues );
                        MysqlEngine::escapesqlcmd( fullsqlcmd, engine, sqlcmd, escapevalues );
                        rc = engine->insert( fullsqlcmd, insertid );
                        if ( !rc && engine->errorcode() == 1062 ) {
                            rc = true;
                            LOG_TRACE( "RECOVER_FROM_FILE('{}') : skip the BinLogger's Line({}) .\n", path, count );
                        }
                    }
                } else {
                    for ( size_t i = 0; i < results->size(); ++i ) {
                        if ( i + 1 < offset2 ) {
                            continue;
                        }

                        uint32_t naffected = 0;
                        ISQLData * d = prototype->data( tablename );
                        assert( d != nullptr && "Can't Found ProtoType" );

                        // 解析数据
                        d->parse( results->row( i ) );
                        // 回调脚本
                        d->invoke( prototype->state(), script );

                        // 回写到数据库中
                        sqlcmd.clear();
                        fullsqlcmd.clear();
                        escapevalues.clear();
                        d->update( sqlcmd, escapevalues );
                        MysqlEngine::escapesqlcmd( fullsqlcmd, engine, sqlcmd, escapevalues );
                        rc = engine->update( fullsqlcmd, naffected );
                        delete d;
                        if ( !rc ) {
                            nloop = i + 1;
                            break;
                        }
                    }
                }

                delete results;
            } break;

            default:
                break;
        }

        delete data;

        if ( !rc ) {
            // 写入CURRENT文件
            STORE_CURRENT_FILE( path, count, nloop );
            break;
        }
    }

    close( fd );
    munmap( filecontent, filesize );
    LOG_INFO( "RECOVER_FROM_FILE('{}') : sync {} Write-OP(s) to MysqlServer .\n", path, count );

    return rc;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

BinLogger::BinLogger()
    : m_CreateTimestamp( 0 ),
      m_RecentFile( nullptr ),
      m_CurrentFile( nullptr )
{
    m_CacheSize = 65536;
    m_Cache = (char *)malloc( m_CacheSize );
}

BinLogger::~BinLogger()
{
    if ( m_Cache != nullptr ) {
        free( m_Cache );
        m_Cache = nullptr;
        m_CacheSize = 0;
    }
}

bool BinLogger::initialize()
{
    // 确保Meta目录存在
    utils::PathUtils::mkdirp( APP_META_PATH );
    return true;
}

void BinLogger::finalize( bool isnormal )
{
    if ( m_RecentFile != nullptr ) {
        delete m_RecentFile;
        m_RecentFile = nullptr;
    }

    if ( m_CurrentFile != nullptr ) {
        m_CurrentFile->sync();
        delete m_CurrentFile;
        m_CurrentFile = nullptr;
    }

    // 正常模式下删除Meta目录下的所有日志文件
    if ( isnormal ) {
        std::vector<std::string> logfiles;
        utils::PathUtils( APP_META_PATH ).traverse( logfiles, BinLogger::suffix().c_str() );

        for ( const auto & lp : logfiles ) {
            // 删除
            remove( lp.c_str() );
            // 同步global服务器
            SYNCFILECOMMAND<DeleteBinFileCommand>( lp );
            // 记录日志
            LOG_INFO( "BinLogger::finalize() : clear MetaFile('{}') succeed .\n", lp.c_str() );
        }
        // 清空CURRENT
        remove( BinLogger::currentfile().c_str() );
    }
}

void BinLogger::rotate( int64_t now )
{
    if ( m_CreateTimestamp == 0
        || m_CurrentFile == nullptr ) {
        createLogfile( now );
    }

    uint32_t interval = 0;

    // 时间间隔=Cache生命周期+刷新间隔×2
    interval += g_AppConfig.getCacheLifetime();
    interval += g_AppConfig.getFlushInterval() * 2;

    if ( now - m_CreateTimestamp <= interval ) {
        return;
    }

    if ( m_RecentFile != nullptr ) {
        std::string removefile = m_RecentFile->path();

        delete m_RecentFile;
        remove( removefile.c_str() );
        // 向集群服务器同步
        SYNCFILECOMMAND<DeleteBinFileCommand>( removefile );
    }

    m_CurrentFile->sync();
    m_CurrentFile->close();
    m_RecentFile = m_CurrentFile;

    createLogfile( now );
}

std::string BinLogger::suffix()
{
    char suffix[64];

    if ( MASTER.instid() == 0 ) {
        strncpy( suffix, "BINLOG", 63 );
    } else {
        snprintf( suffix, 63, "%u.BINLOG", MASTER.instid() );
    }

    return suffix;
}

std::string BinLogger::currentfile()
{
    std::string current_file;

    utils::Utility::snprintf(
        current_file, 1023, "%s/CURRENT", APP_META_PATH );

    return current_file;
}

bool BinLogger::isRecover( const char * path )
{
    // 读取日志文件
    std::vector<std::string> logfiles;
    utils::PathUtils( path ).traverse(
        logfiles, BinLogger::suffix().c_str() );

    return !logfiles.empty();
}

bool BinLogger::recover( MysqlEngine * engine, const char * path )
{
    // 读取日志文件
    std::vector<std::string> logfiles;
    utils::PathUtils( path ).traverse(
        logfiles, BinLogger::suffix().c_str() );

    // 日志文件排序
    std::sort( logfiles.begin(), logfiles.end() );

    TableProtoType prototype;
    std::string curfile;
    size_t offset1 = 0, offset2 = 0;

    // 初始化prototype
    prototype.initialize();
    // 读取CURRENT
    LOAD_CURRENT_FILE( curfile, offset1, offset2 );
    // 恢复
    for ( const auto & lp : logfiles ) {
        if ( lp >= curfile ) {
            bool is_current = lp == curfile;

            if ( !RECOVER_FROM_FILE(
                     engine, &prototype, lp.c_str(), is_current ? offset1 : 0, is_current ? offset2 : 0 ) ) {
                prototype.finalize();
                return false;
            }

            if ( is_current ) {
                remove( BinLogger::currentfile().c_str() );
            }
        }

        // NOTICE: 没办法同步到global服务器上
        remove( lp.c_str() );
        LOG_INFO( "BinLogger::recover() : Not synchronizing(FILE:'{}') to the remote server .\n", lp );
    }
    prototype.finalize();

    return true;
}

void BinLogger::createLogfile( int64_t now )
{
    char path[1024];

    if ( MASTER.instid() == 0 ) {
        snprintf( path, 1023, "%s/%" PRId64 ".BINLOG", APP_META_PATH, now );
    } else {
        snprintf( path, 1023, "%s/%" PRId64 ".%u.BINLOG", APP_META_PATH, now, MASTER.instid() );
    }

    m_CreateTimestamp = now;
    m_CurrentFile = new utils::WritableFile( path );
    assert( m_CurrentFile != nullptr && "create WritableFile failed" );

    // 向集群服务器同步
    SYNCFILECOMMAND<CreateBinFileCommand>( m_CurrentFile->path() );
}

} // namespace data
