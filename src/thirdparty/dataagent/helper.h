
#ifndef __SRC_DATAAGENT_HELPER_H__
#define __SRC_DATAAGENT_HELPER_H__

#include <map>
#include <deque>
#include <functional>

#include "base/types.h"
#include "base/database.h"
#include "base/endpoint.h"
#include "base/message.h"
#include "utils/types.h"
#include "utils/slice.h"
#include "utils/xtime.h"

#include "agent.h"
#include "lua/kaguya.hpp"
#include "scheme/sqlbind.h"

namespace data {

// 查询索引管理器
class DataManager;
class SchemeHelper
{
public:
    SchemeHelper() = default;
    ~SchemeHelper() = default;
    typedef std::function<void( SchemeHelper * )> Register;

private:
    // 基于角色ID的查询索引
    struct IDataRegister {
        IDataRegister() = default;
        virtual ~IDataRegister() = default;
        virtual UnitID get( void * arg ) const = 0;
        virtual void set( void * arg, UnitID id ) = 0;
    };

    template<typename T>
        struct DataRegister : public IDataRegister
        {
            DataRegister( UnitID ( T::*get )() const, void ( T::*set )( UnitID ) )
                : getter( get ), setter( set ) {}
            virtual ~DataRegister() {}
            virtual UnitID get( void * arg ) const { return ( ( (T *)arg )->*getter )(); }
            virtual void set( void * arg, UnitID id ) { ( ( (T *)arg )->*setter )( id ); }
            UnitID ( T::*getter )() const;
            void ( T::*setter )( UnitID );
        };

public:
    // 初始化
    void initialize( const TableProtoType * proto, const Register & regschemes );
    void finalize();
    // 是否是系统表
    bool isSysTable( const Tablename & table ) const;
    // 是否是用户表
    bool isUserTable( const Tablename & table ) const;
    // 获取角色ID
    UnitID getRoleID( ISQLData * data ) const;
    // 获取查询索引
    ISQLData * fillQueryer( UnitID id, const Tablename & table ) const;
    // 获取查询列表
    void fillQueryerList( UnitID id, HostType type, std::vector<ISQLData *> & qs ) const;
    // 注册角色表
    template<typename T>
        void regUserTable( HostType type,
            UnitID ( T::*get )() const = &T::get_db_roleid, void ( T::*set )( UnitID ) = &T::set_db_roleid )
        {
            m_UserTables.emplace(
                T::TABLENAME, std::make_pair( HostTypes{ type }, new DataRegister<T>( get, set ) ) );
        }
    template<typename T>
        void regUserTable( const HostTypes & types,
            UnitID ( T::*get )() const = &T::get_db_roleid, void ( T::*set )( UnitID ) = &T::set_db_roleid )
        {
            m_UserTables.emplace(
                T::TABLENAME, std::make_pair( types, new DataRegister<T>( get, set ) ) );
        }

private:
    typedef std::pair<HostTypes, IDataRegister *> RegisterPair;
    const TableProtoType * m_Prototype = nullptr;
    std::vector<Tablename> m_SysTables;
    std::map<Tablename, RegisterPair> m_UserTables;
};

// 脏数据缓存
class DirtyCache
{
public:
    typedef std::function<void( ISQLData * )> BlockDestroyer;

    DirtyCache( const BlockDestroyer & destroyer ) : m_Destroyer( destroyer ) {}
    ~DirtyCache()
    {
        for ( auto & dirty : m_DirtyTable ) {
            for ( auto & data : dirty.second ) {
                m_Destroyer( data );
            }
        }
        m_DirtyTable.clear();
    }

    void insert( ISQLData * block )
    {
        auto result = m_DirtyTable.find( block->tablename() );
        if ( result == m_DirtyTable.end() ) {
            m_DirtyTable.emplace(
                block->tablename(),
                std::vector<ISQLData *>{ block } );
        } else {
            auto & datalist = result->second;
            if ( datalist.end()
                == std::find( datalist.begin(), datalist.end(), block ) ) {
                datalist.push_back( block );
            }
        }
    }

    template<class Fn>
        Fn inspect( const std::vector<Tablename> & tables, Fn f )
        {
            for ( const auto & table : tables ) {
                auto result = m_DirtyTable.find( table );
                if ( result != m_DirtyTable.end() ) {
                    auto & datalist = result->second;
                    for ( auto it = datalist.begin(); it != datalist.end(); ) {
                        if ( !f( *it ) ) {
                            ++it;
                        } else {
                            it = datalist.erase( it );
                        }
                    }
                }
            }
            return f;
        }

private:
    BlockDestroyer m_Destroyer;
    UnorderedMap<Tablename, std::vector<ISQLData *>> m_DirtyTable;
};

// 结果处理器
class ResultsProcessor
{
public:
    using lua_ref = kaguya::LuaRef;
    using lua_fn = kaguya::LuaFunction;
    using cpp_fn = std::function<void( const Tablename & table, const Slices & results )>;

    struct Processor{
    private:
        enum class fn_type { cpp, lua };

    public:
        Processor( cpp_fn fn )
            : type( fn_type::cpp ), cppfn( fn ) {}
        Processor( lua_ref self, lua_fn fn )
            : type( fn_type::lua ), self( self ), luafn( fn ) {}
        void operator()( const Tablename & table, const Slices & results ) {
            switch ( type ) {
                case fn_type::lua : luafn( self, table, results ); break;
                case fn_type::cpp : std::invoke( cppfn, table, results ); break;
            }
        }

    private:
        fn_type type;
        lua_ref self;
        lua_fn luafn;
        cpp_fn cppfn;
    };

    ResultsProcessor() {}
    ~ResultsProcessor() {}

public:
    // 是否有结果
    bool has( const Tablename & table ) const;
    // 绑定数据处理器
    void unbind( const Tablename & table );
    void bind( const Tablename & table, const cpp_fn & pf );
    void bind( const Tablename & table, const lua_ref & self, const lua_fn & pf );
    // 处理数据
    void process( const Tablename & table, const Slices & results );

private:
    UnorderedMap<Tablename, std::pair<bool, Processor>> m_Processors;
};

// 数据查询阶段
enum class QueryStage {
    None = 0,     // 非法
    Begin = 1,    // 开始查询
    Datasets = 2, // 数据返回
    Timeout = 3,  // 超时
    Complete = 4, // 查询完成
};
using GeneralQueryCallback = std::function<void (QueryStage, const Tablename &, const Slices &)>;

// 通用查询适配器
class GeneralQueryAdapter final : public IDataAdapter
{
public :
    GeneralQueryAdapter( const GeneralQueryCallback & callback )
        : m_Callback( callback ) { m_Callback( QueryStage::Begin, "", {} ); }
    virtual ~GeneralQueryAdapter() final {}
    virtual void autoincr( const Slice & result ) final {};
    virtual void complete() final { m_Callback( QueryStage::Complete, "", {} ); delete this; }
    virtual void timeout() final { m_Callback( QueryStage::Timeout, "", {} ); delete this; }
    virtual void datasets( const Tablename & table, const Slices & results ) { m_Callback( QueryStage::Datasets, table, results ); }

private :
    GeneralQueryCallback m_Callback;
};

} // namespace data

#endif
