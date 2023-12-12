#pragma once

#include <unordered_map>
#include <unordered_set>

#include "base/types.h"
#include "base/database.h"

#include "utils/types.h"
#include "utils/timer.h"

#include "hash.h"
#include "server.h"

struct SSMessage;

namespace data {

class ISQLData;

// 数据块
class DataBlock final : public utils::ITimerEvent
{
public:
    DataBlock( const Tablename & table );
    virtual ~DataBlock() final;
    virtual void onEnd() final;
    virtual bool onTimer( uint32_t & timeout ) final;

public:
    // 更新操作
    void updateMethod( DBMethod method );
    // 调度开始
    void schedule( int32_t interval );
    // 落地
    bool flush2DB( TransID transid = 0 );

public:
    // 操作方式
    DBMethod getMethod() const { return m_DataBlock->get_method(); }
    void setMethod( DBMethod method ) { m_DataBlock->set_method( method ); }

    // 索引字符串
    inline std::string getIdxString() const
    {
        std::string key;
        m_DataBlock->indexstring( eCodec_All, key );
        return key;
    }

    // 数据块
    ISQLData * getDataBlock() const { return m_DataBlock; }
    void setDataBlock( ISQLData * data ) { m_DataBlock = data; }

private:
    ISQLData * m_DataBlock; // 数据块
    int64_t m_Timestamp;    // 时间戳
};

// 索引条目
class IndexEntry
{
public:
    enum Status
    {
        eStatus_None = 0,       // 非法
        eStatus_Complete = 1,   // 完整的索引
        eStatus_InComplete = 2, // 不完整的索引
    };
    using Keys = std::unordered_set<std::string>;

public:
    IndexEntry( const std::string & idxstr, const Tablename & table );
    ~IndexEntry();

public:
    // 状态
    Status getStatus() const { return m_Status; }
    void setStatus( Status status ) { m_Status = status; }
    // 索引字符串
    const std::string & index() const { return m_Index; }
    // 获取表名
    const std::string & table() const { return m_Table; }
    // 添加/删除主键
    void add( const std::string & key );
    void del( const std::string & key );
    // 是否存在
    bool has( const std::string & key ) const;
    // 获取所有主键
    const Keys & datakeys() const { return m_DataKeys; }

private:
    Status m_Status;     // 状态
    std::string m_Index; // 索引字符串(Query)
    Tablename m_Table;   // 表名
    Keys m_DataKeys;     // 主键集合
};

// 表缓存
class TableCache
{
public:
    enum
    {
        eMaxBucketCount = 500000, // 最大桶个数
    };
    typedef std::vector<SSMessage *> MessageQueue;

    TableCache( const Tablename & table, bool iscomplete );
    ~TableCache();

    // 是否完整
    bool isComplete() const { return m_IsComplete; }
    // 表名
    const Tablename & table() const { return m_Table; }

public:
    // 移除数据块
    void removeBlock( const std::string & key );
    // 获取数据块
    DataBlock * getBlock( const std::string & key ) const;
    // 删除数据块
    void destroyBlock( const std::string & key, DataBlock * block );
    // 创建数据块
    DataBlock * createBlock( ISQLData * data, const std::string & key );

    // 刷新整张表到数据库中
    void flush2DB();
    // 打包所有数据
    void datasets( Slices & datas, const ISQLData * query = nullptr );

    // 数据块的个数
    size_t datasize() const { return m_DataCache.size(); }

    // 是否有消息
    bool isEmpty( const std::string & key ) const;
    // 添加消息到队列中
    void pushMessage( const std::string & key, SSMessage * msg );
    // 取走消息
    void popMessages( const std::string & key, MessageQueue & msgqueue );

public:
    // 移除索引条目
    void removeEntry( const std::string & idx );
    // 获取索引条目
    IndexEntry * getEntry( const std::string & idx ) const;
    // 创建索引条目
    IndexEntry * createEntry( const std::string & idx );

    // 索引条目的个数
    size_t indexsize() const { return m_IndexCache.size(); }

    // 是否发生异常
    bool isException( const std::string & idxstr ) const;
    // 处理异常
    void fixException( const std::string & idxstr );
    // 抛出异常
    void throwException( const std::string & idxstr );
    // 获取异常的个数
    size_t getExceptionCount() const { return m_ExceptionTable.size(); }

private:
    using ExceptionTable = std::unordered_map<std::string, time_t>;
    using IndexHashMap = std::unordered_map<std::string, IndexEntry *, HashMurMur64>;
    using DataHashMap = std::unordered_map<std::string, DataBlock *, HashMurMur64>;
    using MessageQueueMap = std::unordered_map<std::string, MessageQueue, HashMurMur64>;

private:
    Tablename m_Table;
    bool m_IsComplete;               // 是否默认完整
    DataHashMap m_DataCache;         // 数据缓存
    IndexHashMap m_IndexCache;       // 索引缓存
    ExceptionTable m_ExceptionTable; // 数据异常表(索引字符串)
    MessageQueueMap m_MessageQueueMap;
};

} // namespace data

