
#ifndef __SRC_DATASERVER_LOGGER_H__
#define __SRC_DATASERVER_LOGGER_H__

#include <cstdio>

#include "io/service.h"

#include "base/types.h"
#include "base/database.h"

#include "utils/slice.h"
#include "utils/xtime.h"
#include "utils/streambuf.h"

#include "message/dbprotocol.h"

#include "config.h"
#include "server.h"

class MysqlEngine;

namespace utils {
class WritableFile;
}

namespace data {

class DataBlock;
class BinLogger
{
public:
    BinLogger();
    ~BinLogger();

public:
    bool initialize();
    void finalize( bool isnormal );
    template<typename T> bool append( T * command );

    // 获取后缀
    static std::string suffix();
    // 获取CURRENT文件
    static std::string currentfile();
    // 是否需要恢复
    static bool isRecover( const char * path );
    // 恢复
    static bool recover(
        MysqlEngine * engine, const char * path );

private:
    // 轮换
    void rotate( int64_t now );

    // 新建日志文件
    void createLogfile( int64_t now );

private:
    int64_t m_CreateTimestamp;           // 日志创建时间
    utils::WritableFile * m_RecentFile;  // 最近的日志文件
    utils::WritableFile * m_CurrentFile; // 当前的日志文件
    char * m_Cache;                      // 缓存
    size_t m_CacheSize;                  // 缓存长度
};

template<typename T>
inline bool BinLogger::append( T * command )
{
    int64_t now = utils::time();

    // 检查日志是否需要轮换
    rotate( now );

    if ( m_CurrentFile == nullptr ) {
        return false;
    }

    // 序列化并且写日志
    StreamBuf pack( m_Cache, m_CacheSize );
    command->append( pack, now );

    // 避免重新分配内存
    if ( m_Cache != pack.data()
        || m_CacheSize != pack.length() ) {
        m_Cache = pack.data();
        m_CacheSize = pack.length();
    }

    Slice data = pack.slice();

    if ( m_CurrentFile->append( data ) ) {
        HostID id = g_AppConfig.getJchHostID( HostType::Logger, g_AppConfig.getZoneID() );
        if ( id != 0 ) {
        // 向日志服务器同步
        SyncBinFileCommand cmd;
        cmd.binlog = data;
            cmd.zoneid = g_AppConfig.getZoneID();
        cmd.filename = m_CurrentFile->path().substr( m_CurrentFile->path().find_last_of( '/' ) + 1 );
            g_HarborService->sendByHostID( id, &cmd );
        }

        return m_CurrentFile->flush();
    }

    return false;
}

#define g_BinLogger GameApp::instance().getBinLogger()

} // namespace data

#endif
