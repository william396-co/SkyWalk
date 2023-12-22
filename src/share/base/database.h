#pragma once

#include <vector>
#include <string>
#include <stdint.h>
#include <format>

#include "utils/slice.h"

enum class DBMethod
{
    None = 0,
    Insert = 1,     // 插入
    Query = 2,      // 查询
    Update = 3,     // 更新
    Remove = 4,     // 删除
    Replace = 5,    // 替换
    QueryBatch = 6, // 批量查询
    Invoke = 7,     // 执行脚本
    Failover = 8,   // 消除异常
};

// 表名
typedef std::string Tablename;

// 脏字段标志
typedef std::vector<int8_t> DirtyFields;

// 查询单元
struct QueryUnit
{
    std::string table;    // 表名
    std::string indexstr; // 索引字符串
    Slice dbdata;         // 查询字段
};

// 表索引
struct TableIndexEntry
{
    Tablename table;
    std::string indexstr;

    TableIndexEntry()
    {}

    TableIndexEntry( const Tablename & t, const std::string idxstr )
        : table( t ),
          indexstr( idxstr )
    {}

    bool operator==( const TableIndexEntry & entry ) const
    {
        return entry.table == table
            && entry.indexstr == indexstr;
    }
};

// SQLCMD
struct SQLCommand
{
    std::string cmd;
    std::vector<std::string> params;

    SQLCommand() = default;

    SQLCommand( std::string && c, std::vector<std::string> && ps )
        : cmd( std::move(c) ),
          params( std::move(ps) )
    {}

    SQLCommand( const std::string & c, const std::vector<std::string> & ps )
        : cmd( c ),
          params( ps )
    {}
};

typedef std::vector<SQLCommand> SQLCommands;

inline DBMethod shift_dbmethod( DBMethod from, DBMethod to )
{
    DBMethod method = DBMethod::None;

    switch ( from ) {
        case DBMethod::Insert: {
            switch ( to ) {
                case DBMethod::Update: method = DBMethod::Insert; break;
                case DBMethod::Remove: method = DBMethod::None; break;
                case DBMethod::Insert: method = DBMethod::Insert; break;
                case DBMethod::Replace: method = DBMethod::Insert; break;
                case DBMethod::Invoke: method = DBMethod::Insert; break;
                default: method = from; break;
            }
        } break;

        case DBMethod::Update: {
            switch ( to ) {
                case DBMethod::Remove: method = DBMethod::Remove; break;
                case DBMethod::Insert: method = DBMethod::Update; break;
                case DBMethod::Update: method = DBMethod::Update; break;
                case DBMethod::Replace: method = DBMethod::Update; break;
                case DBMethod::Invoke: method = DBMethod::Update; break;
                default: method = from; break;
            }
        } break;

        case DBMethod::Remove: {
            switch ( to ) {
                case DBMethod::Insert: method = DBMethod::Update; break;
                case DBMethod::Update: method = DBMethod::Insert; break;
                case DBMethod::Remove: method = DBMethod::Remove; break;
                case DBMethod::Replace: method = DBMethod::Update; break;
                case DBMethod::Invoke: method = DBMethod::Insert; break;
                default: method = from; break;
            }
        } break;

        case DBMethod::Replace: {
            switch ( to ) {
                case DBMethod::Insert: method = DBMethod::Replace; break;
                case DBMethod::Update: method = DBMethod::Replace; break;
                case DBMethod::Remove: method = DBMethod::Remove; break;
                case DBMethod::Replace: method = DBMethod::Replace; break;
                case DBMethod::Invoke: method = DBMethod::Replace; break;
                default: method = from;
                    break;
            }
        } break;

        case DBMethod::Invoke: {
            switch ( to ) {
                case DBMethod::Remove: method = DBMethod::Remove; break;
                case DBMethod::Insert: method = DBMethod::Update; break;
                case DBMethod::Update: method = DBMethod::Update; break;
                case DBMethod::Replace: method = DBMethod::Replace; break;
                case DBMethod::Invoke: method = DBMethod::Update; break;
                default: method = from; break;
            }
        } break;

        default:
            method = to;
            break;
    }

    return method;
}

template<> struct std::formatter<DBMethod> {
    template <typename ParseContext>
        constexpr auto parse( ParseContext & pc ) -> decltype( pc.begin() )  {
        return pc.begin();
    }

    template <typename FormatContext>
        auto format( const DBMethod & value, FormatContext & fc ) const -> decltype( fc.out() ) {
            return std::format_to( fc.out(), "{}", (int32_t)value );
        }
};
