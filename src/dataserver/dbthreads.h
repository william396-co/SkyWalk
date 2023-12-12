#pragma once

#include <deque>
#include <vector>
#include <string>
#include <mutex>

#include "base/types.h"
#include "base/database.h"

#include "io/io.h"
#include "utils/thread.h"
#include "mysql/engine.h"

#include "server.h"

namespace data {

class QueryResultsHandler final : public IResultHandler
{
public:
    QueryResultsHandler(
        sid_t sid,
        TransID transid,
        const std::string & idxstr, ISQLData * prototype );
    virtual ~QueryResultsHandler() final;
    // 预留内存
    virtual void reserve( size_t capacity ) final;
    // 处理数据
    virtual void process( const Slices & results ) final;

public:
    // 返回结果
    void send();
    // 所有结果集合
    std::vector<ISQLData *> & blocks() { return m_BlockList; }

private:
    sid_t m_Sid;
    TransID m_TransID;
    std::string m_IndexString;
    ISQLData * m_ProtoType;
    Slices m_Results;
    std::vector<ISQLData *> m_BlockList;
};

struct DBCommand
{
    DBMethod cmd;
    Tablename table;
    TransID transid;
    std::string idxstr;
    SQLCommand command;
    QueryResultsHandler * handler;

    DBCommand( const Tablename & t,
        DBMethod c, TransID id, const std::string & idx,
        std::string && sqlcmd, std::vector<std::string> && values, QueryResultsHandler * h )
        : cmd( c ),
          table( t ),
          transid( id ),
          idxstr( idx ),
          command( std::move(sqlcmd), std::move(values) ),
          handler( h )
    {}
};

class ReserveQueue;
class DBThread final : public utils::IThread
{
public:
    DBThread( uint8_t index );
    virtual ~DBThread() final;
    virtual bool onStart() final;
    virtual void onExecute() final;
    virtual void onStop() final;

public:
    uint8_t getIndex() const { return m_Index; }
    // 提交cmd
    bool push2Database( DBCommand * command );

private:
    enum
    {
        eEngine_KeepaliveSeconds = 60 * 1000, // 1分钟Keepalive一次
        eEachFrameSeconds = 8,                // 8ms一帧
    };

    void process();
    // 插入
    void insert( const Tablename & table,
        const std::string & idxstr, uint32_t transid,
        const std::string & sqlcmd, const std::vector<std::string> & values );
    // 查询
    void query( const Tablename & table,
        const std::string & idxstr, uint32_t transid,
        const std::string & sqlcmd, const std::vector<std::string> & values, QueryResultsHandler * handler );
    // 更新
    void update( const Tablename & table, const std::string & idxstr,
        const std::string & sqlcmd, const std::vector<std::string> & values );
    // 删除
    void remove( const Tablename & table, const std::string & idxstr,
        const std::string & sqlcmd, const std::vector<std::string> & values );
    // 替换
    void replace( const Tablename & table,
        const std::string & idxstr, uint32_t transid,
        const std::string & sqlcmd, const std::vector<std::string> & values );
    // 异常恢复
    void failover( const Tablename & table, const std::string & idxstr );

private:
    uint8_t m_Index;
    std::mutex m_Lock;
    std::deque<DBCommand *> m_Queue;
    MysqlEngine * m_Engine;
    ReserveQueue * m_ReserveQueue;
    int64_t m_KeepaliveTime;
};

class DBThreads
{
public:
    DBThreads( uint8_t nthreads );
    ~DBThreads();

public:
    bool initialize();
    void finalize();

    // 检查DBThreads
    bool check();
    // 提交任务
    bool post( const Tablename & table,
        const std::string & indexkey, DBMethod cmd, TransID transid,
        std::string && sqlcmd = {}, std::vector<std::string> && values = {}, QueryResultsHandler * handler = nullptr );

private:
    uint8_t m_Number;
    std::vector<DBThread *> m_Threads;
};

#define g_DBThreads GameApp::instance().getDBThreads()

} // namespace data

