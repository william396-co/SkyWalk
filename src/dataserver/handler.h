
#ifndef __SRC_DATASERVER_HANDLER_H__
#define __SRC_DATASERVER_HANDLER_H__

#include "io/io.h"
#include "base/message.h"
#include "base/database.h"

class SSMessage;
class MysqlResults;
class NtpdateCommand;

namespace data {

class ISQLData;
class DataBlock;
class IndexEntry;
class TableCache;
class DataResultTrans;

class CommandHandler
{
public:
    // 处理器
    static void process( SSMessage * msg );
    // 自增
    static void doAutoIncrease( TransID transid, const std::string & lastid );
    // 数据异常
    static void doDataException( const Tablename & tablename, const std::string & indexstring );
    // 数据集
    static void doQueryResults( TransID transid, const std::vector<ISQLData *> & blocks );
    static void doQueryDatablock( TransID transid,
        const Tablename & table, const std::vector<std::vector<std::string>> & results );

private:
    // 握手
    static void handshake( SSMessage * msg );
    // 插入
    static void insert( SSMessage * msg );
    // 查询
    static void query( SSMessage * msg );
    // 更新
    static void update( SSMessage * msg, int32_t mode = 0 );
    // 删除
    static void remove( SSMessage * msg, int32_t mode = 0 );
    // 替换
    static void replace( SSMessage * msg, int32_t mode = 0 );
    // 批量查询
    static void queryBatch( SSMessage * msg );
    // 执行脚本
    static void invoke( SSMessage * msg, int32_t mode = 0 );
    // 强制存档
    static void flush( SSMessage * msg );
    // 服务器状态
    static void status( SSMessage * msg );
    // 同步时间
    static void ntpdate( SSMessage * msg );
    // 清除异常
    static void failover( SSMessage * msg );

private:
    // 插入缓存中
    static DataBlock * realInsertCache( ISQLData * block );
    // 存档
    static void flushIndexEntry( IndexEntry * entry, TableCache * cache, bool isdrop );
    static void flushIndexEntry( const Tablename & table, int8_t dropflag, const std::string & indexstring );
    // 真实查询
    static void realQuery( sid_t sid, MessageID cmd, TransID transid,
        const Tablename & table, const std::string & idxstr, const Slice & dbdata, uint32_t timeout );
    // 查询DataBlock
    static bool queryDatablock( const Tablename & table, const std::string & keystr, const Slice & dbdata );
    static bool queryDatablock( ISQLData * data, const std::string & keystr, const std::string & idxstr );
    // 执行消息队列中的消息
    static void doMessageQueue( TableCache * cache, const std::string & keystr );
};

} // namespace data

#endif
