
#ifndef __SRC_DATAAGENT_IMPL_AGENTIMPL_H__
#define __SRC_DATAAGENT_IMPL_AGENTIMPL_H__

#include "io/io.h"
#include "utils/types.h"
#include "querycache.h"
#include "dataagent/agent.h"

namespace utils {
class TransactionManager;
}

namespace data {

class ISQLData;

class TimeTick;
class ResultQueue;
class AgentClient;
class TableProtoType;

class DataAgentImpl final : public IDataAgent
{
public:
    DataAgentImpl(
        uint32_t precision,
        RouterFunction routerfunc,
        ExceptionFunction exceptionfunc, kaguya::State * state );
    virtual ~DataAgentImpl() final;
    // 调度/处理结果
    // 建议每帧调用一次
    virtual void schedule( int64_t now ) final;
    // 是否活跃
    virtual bool check( int64_t now ) const final;
    // 合并脏数据
    virtual ISQLData * merge( ISQLData * d ) final;
    // 返回
    virtual const TableProtoType * prototype() const final { return m_ProtoType; }
    // 阻塞地连接远程服务器
    // 会默认重试10次，相当于超时时间的10倍
    virtual bool connect( const std::string & host, uint16_t port, uint32_t timeout ) final;
    // 插入
    virtual bool insert( ISQLData * d, IDataAdapter * adapter, uint32_t timeout ) final;
    // 更新
    virtual bool update( ISQLData * d ) final;
    // 移除
    virtual bool remove( ISQLData * d ) final;
    // 替换
    virtual bool replace( ISQLData * d, IDataAdapter * adapter, uint32_t timeout ) final;
    // 是否在查询中
    virtual bool isQuery( const Tablename & table, const std::string & idxstr ) final;
    // 查询
    virtual bool query( ISQLData * q, IDataAdapter * adapter, uint32_t timeout ) final;
    // 批量查询
    virtual bool queryBatch( const std::vector<ISQLData *> & qs, IDataAdapter * adapter, uint32_t timeout ) final;
    // 执行脚本
    virtual bool invoke( ISQLData * d, const std::string & script, bool isreplace ) final;
    // 强制存档
    virtual bool flush( const std::vector<ISQLData *> & qs, bool isdrop ) final;
    // 异常恢复
    virtual bool failover( const std::vector<ISQLData *> & qs ) final;
    virtual bool failover( const std::vector<TableIndexEntry> & qs ) final;

public:
    const std::string & schemeversion() const;
    void updateKeepalive( sid_t sid, uint8_t status = 1 );
    uint32_t getTimeoutSeconds() const { return m_Timeout; }
    TimeTick * getTimeTick() const { return m_TimeTick; }
    ResultQueue * getResultQueue() const { return m_ResultQueue; }
    QueryCache * getQueryCache() const { return m_QueryCache; }
    AgentClient * getAgentClient() const { return m_AgentClient; }
    ExceptionFunction getExceptionFunction() const { return m_ExceptionFunction; }
    utils::TransactionManager * getTransactionManager() const { return m_TransManager; }

private:
    struct DataHost
    {
        sid_t sid;
        uint8_t status; // 0: 断开; 1: 正常
        int64_t keepalive;

        DataHost() : sid( 0 ), status( 0 ), keepalive( 0 ) {}
        DataHost( sid_t s, uint8_t st, int64_t t ) : sid( s ), status( st ), keepalive( t ) {}
    };

    bool isAlive() const;
    sid_t route2DataHost( const Tablename & table, const std::string & idxstr ) const;

private:
    uint32_t m_Timeout; // 超时时间
    TimeTick * m_TimeTick;
    TableProtoType * m_ProtoType;
    QueryCache * m_QueryCache;                  // 查询缓存
    AgentClient * m_AgentClient;               // 代理客户端
    ResultQueue * m_ResultQueue;                // 结果队列
    utils::TransactionManager * m_TransManager; // 事务管理器

private:
    std::vector<DataHost> m_DataHostList;
    RouterFunction m_RouterFunction;       // 路由函数
    ExceptionFunction m_ExceptionFunction; // 异常函数
};

} // namespace data

#endif
