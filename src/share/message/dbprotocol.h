
#ifndef __SRC_MESSAGE_DBPROTOCOL_H__
#define __SRC_MESSAGE_DBPROTOCOL_H__

#include "protocol.h"
#include "base/database.h"

enum
{
    eSSCommand_DBHandShake = 0x0201,    // 握手
    eSSCommand_DBInsert = 0x0202,       // 插入
    eSSCommand_DBQuery = 0x0203,        // 查询
    eSSCommand_DBUpdate = 0x0204,       // 更新
    eSSCommand_DBRemove = 0x0205,       // 删除
    eSSCommand_DBReplace = 0x0206,      // 替换
    eSSCommand_DBQueryBatch = 0x0207,   // 批量查询
    eSSCommand_DBInvoke = 0x0208,       // 执行脚本
    eSSCommand_DBFlush = 0x0209,        // 强制存档
    eSSCommand_DBResults = 0x020A,      // 结果集返回
    eSSCommand_DBAutoIncrease = 0x020B, // 自增索引返回
    eSSCommand_DBException = 0x020C,    // 异常
    eSSCommand_DBFailover = 0x020D,     // 恢复异常
    //
    eSSCommand_CreateBinFile = 0x020E, // 创建bin文本
    eSSCommand_SyncBinLog = 0x020F,    // 同步bin数据
    eSSCommand_DeleteBinFile = 0x0210, // 销毁bin数据
};

// 握手协议
MESSAGE_BEGIN( DBHandShakeCommand, eSSCommand_DBHandShake, 256 )
    std::string version; // 版本号
    Endpoints slavelist; // slave机器列表
MESSAGE_END( DBHandShakeCommand, version, slavelist )

// 数据库插入
struct DBInsertCommand : SSMessage, Codec<DBInsertCommand>
{
public:
    TransID transid = 0;  // 事务ID
    std::string table;    // 表名
    uint32_t timeout = 0; // 超时时间
    Slice dbdata;         // 数据段

    DBInsertCommand()
        : SSMessage( eSSCommand_DBInsert ) {}
    virtual ~DBInsertCommand() {
        if ( space == nullptr ) { dbdata.free(); }
    }

    // 追加
    void append( StreamBuf & buf, int64_t timestamp );

    virtual SSMessage * clone( bool isfull ) { return copy( isfull ); }
    virtual Slice encode() { return serialize( 4096 ); }
    virtual bool decode( const Slice & data ) { return unserialize( data, true ); }
};
REFLECTION( DBInsertCommand, transid, table, timeout, dbdata );

// 数据库更新
struct DBUpdateCommand : SSMessage, Codec<DBUpdateCommand>
{
public:
    std::string table;  // 表名
    std::string keystr; // 主键字符串
    Slice dbdata;       // 变化列表

    DBUpdateCommand()
        : SSMessage( eSSCommand_DBUpdate ) {}
    virtual ~DBUpdateCommand() {
        if ( space == nullptr ) { dbdata.free(); }
    }

    // 追加
    void append( StreamBuf & buf, int64_t timestamp );

    virtual SSMessage * clone( bool isfull ) { return copy( isfull ); }
    virtual Slice encode() { return serialize( 1024 ); }
    virtual bool decode( const Slice & data ) { return unserialize( data, true ); }
};
REFLECTION( DBUpdateCommand, table, keystr, dbdata );

// 数据库删除
struct DBRemoveCommand : SSMessage, Codec<DBRemoveCommand>
{
public:
    std::string table;  // 表名
    std::string keystr; // 主键字符串
    Slice dbdata;       // 变化列表

    DBRemoveCommand()
        : SSMessage( eSSCommand_DBRemove ) {}
    virtual ~DBRemoveCommand() {
        if ( space == nullptr ) { dbdata.free(); }
    }

    // 追加
    void append( StreamBuf & buf, int64_t timestamp );

    virtual SSMessage * clone( bool isfull ) { return copy( isfull ); }
    virtual Slice encode() { return serialize( 256 ); }
    virtual bool decode( const Slice & data ) { return unserialize( data, true ); }
};
REFLECTION( DBRemoveCommand, table, keystr, dbdata );

// 数据库替换
struct DBReplaceCommand : SSMessage, Codec<DBReplaceCommand>
{
public:
    TransID transid = 0;  // 事务ID
    std::string table;    // 表名
    uint32_t timeout = 0; // 超时时间
    Slice dbdata;         // 数据段

    DBReplaceCommand()
        : SSMessage( eSSCommand_DBReplace ) {}
    virtual ~DBReplaceCommand() {
        if ( space == nullptr ) { dbdata.free(); }
    }

    // 追加
    void append( StreamBuf & buf, int64_t timestamp );

    virtual SSMessage * clone( bool isfull ) { return copy( isfull ); }
    virtual Slice encode() { return serialize( 1024 ); }
    virtual bool decode( const Slice & data ) { return unserialize( data, true ); }
};
REFLECTION( DBReplaceCommand, transid, table, timeout, dbdata );

// 数据库调用脚本
struct DBInvokeCommand : SSMessage, Codec<DBInvokeCommand>
{
public:
    std::string table;      // 表名
    std::string keystr;     // 主键字符串
    std::string script;     // 脚本
    Slice dbdata;           // 变化列表
    bool isreplace = false; // 数据不存在时创建

    DBInvokeCommand()
        : SSMessage( eSSCommand_DBInvoke ) {}
    virtual ~DBInvokeCommand() {
        if ( space == nullptr ) { dbdata.free(); }
    }

    // 追加
    void append( StreamBuf & buf, int64_t timestamp );

    virtual SSMessage * clone( bool isfull ) { return copy( isfull ); }
    virtual Slice encode() { return serialize( 512 ); }
    virtual bool decode( const Slice & data ) { return unserialize( data, true ); }
};
REFLECTION( DBInvokeCommand, table, keystr, script, dbdata, isreplace );

// 数据库查询
struct DBQueryCommand : SSMessage, Codec<DBQueryCommand>
{
public:
    TransID transid = 0;    // 事务ID
    std::string table;      // 表名
    std::string indexstr;   // 索引字符串
    uint32_t timeout = 0;   // 超时时间
    Slice dbdata;           // 查询字段

    DBQueryCommand()
        : SSMessage( eSSCommand_DBQuery ) {}
    virtual ~DBQueryCommand() {
        if ( space == nullptr ) { dbdata.free(); }
    }

    virtual SSMessage * clone( bool isfull ) { return copy( isfull ); }
    virtual Slice encode() { return serialize( 1024 ); }
    virtual bool decode( const Slice & data ) { return unserialize( data, true ); }
};
REFLECTION( DBQueryCommand, transid, table, indexstr, timeout, dbdata );

// 数据库批量查询
struct DBQueryBatchCommand : SSMessage, Codec<DBQueryBatchCommand>
{
public:
    TransID transid = 0;                // 事务ID
    uint32_t timeout = 0;               // 超时时间
    std::vector<QueryUnit> querylist;   // 查询列表

    DBQueryBatchCommand()
        : SSMessage( eSSCommand_DBQueryBatch ) {}
    virtual ~DBQueryBatchCommand() {
        if ( space == nullptr ) { for ( auto & v : querylist ) { v.dbdata.free(); }
            querylist.clear();
        }
    }

    virtual SSMessage * clone( bool isfull ) { return copy( isfull ); }
    virtual Slice encode() { return serialize( 1024 ); }
    virtual bool decode( const Slice & data ) { return unserialize( data, true ); }
};
REFLECTION( DBQueryBatchCommand, transid, timeout, querylist );

// 强制存档
MESSAGE_BEGIN( DBFlushCommand, eSSCommand_DBFlush, 512 )
    int8_t dropflag = 0;                    // 是否丢弃内存数据0-不丢弃1-丢弃
    std::vector<TableIndexEntry> entrylist; // 需要落地的所有索引
MESSAGE_END( DBFlushCommand, dropflag, entrylist )

// 数据库结果
struct DBResultsCommand : SSMessage, Codec<DBResultsCommand>
{
public:
    TransID transid = 0;  // 事务ID
    std::string table;    // 表名
    std::string indexstr; // 主键字符串
    Slices results;       // 结果

    DBResultsCommand()
        : SSMessage( eSSCommand_DBResults ) {}
    virtual ~DBResultsCommand() {
        if ( space == nullptr ) {
            for ( auto & result : results ) { result.free(); }
            results.clear();
        }
    }

    virtual SSMessage * clone( bool isfull ) { return copy( isfull ); };
    virtual Slice encode() { return serialize( 4096 ); }
    virtual bool decode( const Slice & data ) { return unserialize( data, true ); }
};
REFLECTION( DBResultsCommand, transid, table, indexstr, results );

// 数据库自增
struct DBAutoIncreaseCommand : SSMessage, Codec<DBAutoIncreaseCommand>
{
public:
    TransID transid = 0;  // 事务ID
    std::string table;    // 表名
    std::string indexstr; // 索引字符串
    Slice lastid;         // 自增ID

    DBAutoIncreaseCommand()
        : SSMessage( eSSCommand_DBAutoIncrease ) {}
    virtual ~DBAutoIncreaseCommand() {
        if ( space == nullptr ) { lastid.free(); }
    }

    virtual SSMessage * clone( bool isfull ) { return copy( isfull ); }
    virtual Slice encode() { return serialize( 128 ); }
    virtual bool decode( const Slice & data ) { return unserialize( data, true ); }
};
REFLECTION( DBAutoIncreaseCommand, transid, table, indexstr, lastid );

// 数据异常
MESSAGE_BEGIN( DBExceptionCommand, eSSCommand_DBException, 1024 )
    std::string table;    // 表名
    std::string indexstr; // 索引字符串
    std::string dbdata;   // 异常数据
    std::string script;   // 脚本
MESSAGE_END( DBExceptionCommand, table, indexstr, dbdata, script )

// 异常恢复
MESSAGE_BEGIN( DBFailoverCommand, eSSCommand_DBFailover, 1024 )
    std::vector<TableIndexEntry> exceptions; // 异常
MESSAGE_END( DBFailoverCommand, exceptions )

// 通知binserver创建日志文件
MESSAGE_BEGIN( CreateBinFileCommand, eSSCommand_CreateBinFile, 1024 )
    ZoneID zoneid = 0;
    std::string filename;
MESSAGE_END( CreateBinFileCommand, zoneid, filename )

// 同步binserver日志
struct SyncBinFileCommand : SSMessage, Codec<SyncBinFileCommand>
{
public:
    ZoneID zoneid = 0;
    std::string filename;
    Slice binlog;

    SyncBinFileCommand()
        : SSMessage( eSSCommand_SyncBinLog ) {}
    virtual ~SyncBinFileCommand() {}

    virtual SSMessage * clone( bool isfull ) { return copy( isfull ); }
    virtual Slice encode() { return serialize( 1024 ); }
    virtual bool decode( const Slice & data ) { return unserialize( data, true ); }
};
REFLECTION( SyncBinFileCommand, zoneid, filename, binlog );

// 通知binserver删除日志文件
MESSAGE_BEGIN( DeleteBinFileCommand, eSSCommand_DeleteBinFile, 1024 )
    ZoneID zoneid = 0;
    std::string filename;
MESSAGE_END( DeleteBinFileCommand, zoneid, filename )

#endif
