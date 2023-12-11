
#ifndef __SRC_DATAAGENT_IMPL_QUERYCACHE_H__
#define __SRC_DATAAGENT_IMPL_QUERYCACHE_H__

#include <string.h>

#include "base/database.h"
#include "utils/types.h"
#include "utils/transaction.h"

namespace utils {
class TransactionManager;
}

namespace data {

class ISQLData;
class TableProtoType;

class QueryCache
{
public:
    QueryCache( TableProtoType * proto, utils::TransactionManager * manager )
        : m_ProtoType( proto ), m_TransManager( manager ) {}
    ~QueryCache();

public:
    // 合并数据
    ISQLData * mergedirty( const Tablename & table,
        const std::string & keystr, ISQLData * data );

    // 弹出最近的脏数据
    void popdirty( const Tablename & table,
        const std::string & idxstr, std::vector<ISQLData *> & dirtylist );
    //
    void pushdirty( const std::string & keystr,
        const std::string & idxstr, ISQLData * block, const std::string & script = "", bool isreplace = false );

public:
    // 是否正在查询
    bool isQuery( const Tablename & table, const std::string & idxstr );
    // 查询入列和出列操作
    void inqueue( const Tablename & table, const std::string & idxstring, TransID transid );
    void outqueue( const Tablename & table, const std::string & idxstring, TransID transid );
    void clearqueue( const Tablename & table, const std::string & idxstring, TransID transid );

private:
    struct DataEntry
    {
        ISQLData * block = nullptr;
        std::string script;
        bool isreplace = false;

        DataEntry() {}
        DataEntry( ISQLData * d, const std::string & s, bool ir ) : block( d ), script( s ), isreplace( ir ) {}
    };

    struct IndexEntry
    {
        std::vector<TransID> translist;        // 查询的事务列表
        std::vector<std::string> dirtykeylist; // 脏数据的key列表
    };

    typedef std::vector<DataEntry> DataEntries;
    typedef UnorderedMap<std::string, DataEntries> DataCache;
    typedef UnorderedMap<std::string, IndexEntry> IndexCache;

    // 脏数据
    // 添加脏数据列表中
    void add2DirtyList( const Tablename & table,
        const std::string & idxstr, const std::string & datakey );
    ISQLData * mergedirty(
        const std::string & key, ISQLData * data = nullptr );

private:
    DataCache m_DirtyCache;                     // 脏数据缓存
    IndexCache m_QueryCache;                    // 查询索引缓存
    TableProtoType * m_ProtoType;               // 数据表原型
    utils::TransactionManager * m_TransManager; // 事务管理器
};

} // namespace data

#endif
