#pragma once

#include <deque>
#include <unordered_map>

#include "base/types.h"
#include "utils/types.h"
#include "base/database.h"

class MysqlEngine;

namespace utils {
class WritableFile;
}

namespace data {

class ReserveQueue
{
public:
    ReserveQueue( uint8_t index );
    ~ReserveQueue();

    static bool isException( const char * path );

public:
    // 是否出错
    bool isException(
        const Tablename & table,
        const std::string & idxstr ) const;
    // 推送到数据库中
    bool flush2DB( MysqlEngine * engine );
    // 异常恢复
    void failover(
        const Tablename & table,
        const std::string & idxstr );
    // 添加SQLCMD
    void append( const Tablename & table,
        const std::string & idxstr, const std::string & sqlcmd );
    // 查看队列是否空
    bool empty() const { return m_CmdQueue.empty(); }
    // 队列长度
    size_t size() const { return m_CmdQueue.size(); }

private:
    bool save2Logger();
    std::string filename();
    static std::string suffix();
    utils::WritableFile * getErrorLog();

    struct SQLCmdLine
    {
        int64_t timestamp;
        std::string sqlcmd;
        Tablename table;
        std::string indexstring;

        SQLCmdLine()
            : timestamp( 0 )
        {}

        SQLCmdLine( int64_t t, const std::string & sql, const Tablename & table_, const std::string & idxstr )
            : timestamp( t ),
              sqlcmd( sql ),
              table( table_ ),
              indexstring( idxstr )
        {}
    };

    struct DataException
    {
        size_t count;
        time_t timestamp;
        DataException() : count( 0 ), timestamp( 0 ) {}
        DataException( size_t c ) : count( c ), timestamp( 0 ) {}
        DataException( size_t c, time_t now ) : count( c ), timestamp( now ) {}
    };
    using ExceptionTable = std::unordered_map<std::string, DataException>;

    uint8_t m_Index;
    std::deque<SQLCmdLine> m_CmdQueue;
    ExceptionTable m_ExceptionTable;
    utils::WritableFile * m_ErrorLogger;
};

} // namespace data

