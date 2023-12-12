
#ifndef __SRC_DATAAGENT_AGENT_H__
#define __SRC_DATAAGENT_AGENT_H__

#include "base/types.h"
#include "base/database.h"

#include "utils/slice.h"

namespace kaguya {
class State;
}

namespace data {

class ISQLData;
class TableProtoType;

// 异常函数
typedef bool ( *ExceptionFunction )(
    const Tablename & table,
    const Slice & dbdata,
    const std::string & script );

// 路由函数
typedef uint8_t ( *RouterFunction )(
    uint8_t count,
    const Tablename & table,
    const std::string & idxstr );

// 数据适配器
class IDataAdapter
{
public:
    IDataAdapter() {}
    virtual ~IDataAdapter() {}
    // 超时
    virtual void timeout() = 0;
    // 查询完成
    virtual void complete() = 0;
    // 自增ID
    virtual void autoincr( const Slice & result ) = 0;
    // 查询结果返回
    // table    - 表名
    // results  - 查询结果
    virtual void datasets( const Tablename & table, const Slices & results ) = 0;
};

class IDataAgent 
{
public:
    IDataAgent() {}
    virtual ~IDataAgent() {}
    // 创建DataAgent
    // precision            - 定时器精度
    // routerfunction       - 多服务器路由规则
    // exceptionfunction    - 异常函数
    // state                - lua栈
    static IDataAgent * create( uint32_t precision,
        RouterFunction routerfunction = nullptr,
        ExceptionFunction exceptionfunction = nullptr, kaguya::State * state = nullptr );

public:
    // 调度/处理结果
    // 建议每帧调用一次
    virtual void schedule( int64_t now ) = 0;
    // 代理是否活跃
    virtual bool check( int64_t now ) const = 0;
    // 合并查询过程中修改的数据
    virtual ISQLData * merge( ISQLData * d ) = 0;
    // 获取表结构原型
    virtual const TableProtoType * prototype() const = 0;
    // 阻塞地连接远程服务器
    // 会默认重试10次，相当于超时时间的10倍
    virtual bool connect( const std::string & host, uint16_t port, uint32_t timeout ) = 0;
    // 插入
    virtual bool insert( ISQLData * d, IDataAdapter * adapter, uint32_t timeout ) = 0;
    // 更新
    virtual bool update( ISQLData * d ) = 0;
    // 移除
    virtual bool remove( ISQLData * d ) = 0;
    // 替换
    virtual bool replace( ISQLData * d, IDataAdapter * adapter, uint32_t timeout ) = 0;
    // 是否在查询中
    virtual bool isQuery( const Tablename & table, const std::string & idxstr ) = 0;
    // 查询
    virtual bool query( ISQLData * q, IDataAdapter * adapter, uint32_t timeout ) = 0;
    // 批量查询
    virtual bool queryBatch( const std::vector<ISQLData *> & qs, IDataAdapter * adapter, uint32_t timeout ) = 0;
    // 执行脚本
    // replace为真时，当数据块不存在的情况下直接插入
    virtual bool invoke( ISQLData * d, const std::string & script, bool isreplace ) = 0;
    // 强制存档
    // isdrop - 是否丢弃DataServer中的内存数据
    virtual bool flush( const std::vector<ISQLData *> & qs, bool isdrop ) = 0;
    // 异常恢复
    virtual bool failover( const std::vector<ISQLData *> & qs ) = 0;
    virtual bool failover( const std::vector<TableIndexEntry> & qs ) = 0;
};

} // namespace data

#endif
