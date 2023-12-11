
#ifndef __SRC_DATAAGENT_MANAGER_H__
#define __SRC_DATAAGENT_MANAGER_H__

#include <functional>
#include <set>
#include <unordered_set>
#include <type_traits>
#include <utility>
#include "base/types.h"
#include "base/database.h"
#include "base/endpoint.h"

#include "utils/types.h"
#include "utils/singleton.h"

#include "agent.h"
#include "helper.h"
#include "scheme/sqlbind.h"

namespace data {

class DataManager : public Singleton<DataManager>
{
private:
    friend class Singleton<DataManager>;
    DataManager();
    virtual ~DataManager();

public:
    enum Type
    {
        DATA = 1,   // 刷新的是数据
        MEMORY = 2, // 刷新的是内存
    };

    // 初始化/销毁
    // batchcount >  1 合并多个变更提交
    // batchcount == 1 即时提交到数据服务器
    // batchcount == 0 合并一帧内的所有变更提交, 重要
    bool initialize(
        uint32_t precision, uint32_t batchcount = 1,
        RouterFunction router = nullptr, ExceptionFunction exceptioner = nullptr, kaguya::State * state = nullptr );
    void finalize();
    // 注册所有scheme
    void regscheme( const SchemeHelper::Register & reg );
    // 连接
    bool connect( const Endpoint & ep, uint32_t timeout );
    // 获取表结构版本号
    const std::string & schemeversion() const;
    // 获取代理
    data::IDataAgent * agent() const { return m_DataAgent; }
    // 获取表结构助手
    data::SchemeHelper * helper() { return &m_SchemeHelper; }
    // 获取数据原型
    const TableProtoType * prototype() const { return m_DataAgent->prototype(); }

public:
    // 执行SQL语句

    // 插入
    void insert( ISQLData * block );
    // 更新
    void update( ISQLData * block );
    // 删除
    void remove( ISQLData * block, bool isfree = false );
    // 替换
    void replace( ISQLData * block );
    // 查询
    void query( ISQLData * q, int32_t timeout, const GeneralQueryCallback & callback );
    void query( const std::vector<ISQLData *> & qs, int32_t timeout, const GeneralQueryCallback & callback );

    // 强行刷新到数据库中
    void flush( int32_t type = ( DATA | MEMORY ) );

    // 存档
    void save( DBMethod method, ISQLData * block, bool isfree = false );

    // invoke
    void invoke( ISQLData * block, const std::string & script, bool isreplace = false );

public:
    // 创建block
    ISQLData * create( const Tablename & table );
    // 创建block
    template<typename T, typename... Types>
    T * create( Types &&... types ) {
        return new T( std::forward<Types>( types )... );
    }

    // 创建block并解码
    ISQLData * parse( const Tablename & table, const Slice & slice, bool ismerge = true );

    // 创建block并解码
    template<typename T>
        T * parse( const Slice & slice, bool ismerge = true )
        {
            ISQLData * data = parse( T::TABLENAME, slice, ismerge );
            if ( data != nullptr ) {
                return static_cast<T *>( data );
            }
            return nullptr;
        }

    // 销毁系统数据
    bool destroy( ISQLData * data );

private:
    uint32_t m_Limit;
    data::IDataAgent * m_DataAgent;
    UnorderedSet<ISQLData *> m_DestroyDataBlock; // 销毁data
    data::SchemeHelper m_SchemeHelper;  //
    std::vector<std::pair<ISQLData *, DBMethod>> m_DataChanges;
};

} // namespace data

#define g_DataManager data::DataManager::instance()
#define g_DataAgent data::DataManager::instance().agent()
#define g_SchemeHelper data::DataManager::instance().helper()

#endif
