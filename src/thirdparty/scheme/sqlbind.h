#pragma once

#include <set>
#include <vector>
#include <string>
#include <cstring>
#include <unordered_map>

#include "utils/types.h"
#include "utils/slice.h"
#include "utils/hashfunc.h"
#include "base/database.h"

namespace kaguya {
class State;
}

namespace data {

// Codec
enum { eCodec_Dirty = 0, eCodec_All = 1, eCodec_Query = 2 };
// DataSource
enum { eDataSource_Database = 1, eDataSource_Dataserver = 2 };

// 数据表结构
class ISQLData
{
public:
    ISQLData() {}
    virtual ~ISQLData() {}

    // Check Object is dirty
    virtual bool isDirty() const = 0;
    // Check Keys is dirty
    virtual bool isKeydirty() const = 0;
    // TableName
    virtual const Tablename & tablename() const = 0;
    // KeyString
    virtual void keystring( std::string & k ) const = 0;
    // IndexString
    virtual void indexstring( uint8_t op, std::string & k ) const = 0;

    // Parse
    virtual bool parse( const Slices & result ) = 0;
    // AutoIncrease
    // source : eDataSource_Database, eDataSource_Dataserver
    virtual void autoincrease( uint8_t source, const Slice & result ) = 0;

    // Splicing sql statement (query, insert, update, remove, replace)
    virtual bool query(
        std::string & sqlcmd, std::vector<std::string> & escapevalues, bool all = true ) = 0;
    virtual bool insert(
        std::string & sqlcmd, std::vector<std::string> & escapevalues ) = 0;
    virtual bool update(
        std::string & sqlcmd, std::vector<std::string> & escapevalues ) = 0;
    virtual bool remove(
        std::string & sqlcmd, std::vector<std::string> & escapevalues ) = 0;
    virtual bool replace(
        std::string & sqlcmd, std::vector<std::string> & escapevalues ) = 0;

    // Serialize
    // op : eCodec_Dirty, eCodec_All, eCodec_Query
    virtual Slice encode( uint8_t op ) = 0;
    virtual bool decode( const Slice & result, uint8_t op ) = 0;

    // Clean Dirty Fields
    virtual void clean() = 0;
    // Clone
    virtual ISQLData * clone( bool isfull ) const = 0;
    // Block Merge
    virtual void merge( const ISQLData * data ) = 0;
    // Match Block
    virtual bool match( const ISQLData * data ) const = 0;
    // Lua Invoke
    virtual bool invoke( kaguya::State * state, const std::string & script ) = 0;

public:
    // 操作方式
    DBMethod get_method() const { return m_method; }
    void set_method( DBMethod method ) { m_method = method; }

protected:
    DBMethod             m_method;
};

// 表结构原型
class TableProtoType
{
public:
    // 版本号
    static std::string VERSION;
    TableProtoType( kaguya::State * state = nullptr );
    ~TableProtoType();

    // 初始化
    void initialize();
    // 销毁
    void finalize();

public:
    // LUA栈
    kaguya::State * state() const { return m_State; }
    // 获取所有的表名
    const std::vector<Tablename> & tables() const { return m_Tables; }
    // 版本号
    const std::string & schemeversion() const { return TableProtoType::VERSION; }

    bool isComplete( const Tablename & table ) const {
        return m_InCompleteTable.find( table ) == m_InCompleteTable.end();
    }

    // 根据表名获取数据原型
    ISQLData * data( const Tablename & table ) const {
        auto result = m_DataProtoType.find( table );
        if ( result != m_DataProtoType.end() ) {
            return result->second->clone( false );
        }
        return nullptr;
    }

    template<class T>
        T * data() {
            return static_cast<T *>( data( T::TABLENAME ) );
        }

private:
    bool m_CloseState;
    kaguya::State *                         m_State;
    std::vector<Tablename>                  m_Tables;
    std::set<Tablename>                     m_InCompleteTable;
    std::unordered_map<Tablename, ISQLData *>     m_DataProtoType;
};

} // namespace data

