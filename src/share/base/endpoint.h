#pragma once

#include <vector>
#include <string>
#include <cassert>
#include <stdint.h>
#include <format>
#include <sstream>

#include "types.h"

// 单区最大服务器编号
#define HOSTID_BYTES 8
#define HOSTID_MAX ( 1 << ( HOSTID_BYTES - 1 ) )
#define ANY_HOST6 "::"
#define ANY_HOST4 "0.0.0.0"

// 节点类型
enum class HostType
{
    None = 0,   // 非法的主机节点
    Gate = 1,   // 网关服务器
    Scene = 2,  // 场景服务器
    World = 3,  // 世界服务器
    Copy = 4,   // 副本服务器
    Logger = 5, // 日志服务器
    Data = 6,   // 数据服务器
    Login = 7,  // 验证服务器
    Master = 8, // 管理服务器
    // TODO: ....
    Global = 11,  // 全局服务器
    Transit = 12, // 中转服务器
    Cross = 13,   // 跨服服务器
    Any = 255,    // 任何节点
};
typedef std::vector<HostType> HostTypes; // 节点类型列表

// 主机端信息
struct Endpoint
{
    uint16_t port = 0;     // 内网port
    std::string host = ""; // 内网ip

    Endpoint() = default;

    Endpoint( uint16_t port_, int8_t type = 4 )
        : port( port_ )
    {
        if ( type == 4 ) {
            host = ANY_HOST4;
        } else if ( type == 6 ) {
            host = ANY_HOST6;
        }
    }

    Endpoint( const char * host_, uint16_t port_ )
        : port( port_ ),
          host( host_ )
    {}

    Endpoint( const std::string & host_, uint16_t port_ )
        : port( port_ ),
          host( host_ )
    {}

    void clear() { port = 0; host.clear(); }
    bool empty() const { return host.empty() && port == 0; }
    bool operator==( const Endpoint & endpoint ) const { return ( endpoint.host == host && endpoint.port == port ); }
};
typedef std::vector<Endpoint> Endpoints;

struct HostEndpoint
{
    HostID id;             // ID
    HostType type;         // 类型
    Endpoint endpoint;     // 主机地址
    Endpoint extendpoint;  // 外网主机地址
    std::string procmark;  // 进程标识
    HostTypes connectlist; // 连接的服务器类型

    HostEndpoint()
        : id( 0 ),
          type( HostType::None )
    {}

    HostEndpoint( HostID i )
        : id( i ),
          type( HostType::None )
    {}
};
typedef std::vector<HostEndpoint> HostEndpoints;

template<> struct std::formatter<HostType> {
    template <typename ParseContext>
        constexpr auto parse( ParseContext & pc ) -> decltype( pc.begin() )  {
        return pc.begin();
    }

    template <typename FormatContext>
        auto format( const HostType & value, FormatContext & fc ) const -> decltype( fc.out() ) {
            return std::format_to( fc.out(), "{}", (int32_t)value );
        }
};

template<> struct std::formatter<HostTypes>
{
    template<typename ParseContext>
    constexpr auto parse( ParseContext & pc ) -> decltype( pc.begin() )
    {
        return pc.begin();
    }
    template<typename FormatContext>
    auto format( const HostTypes & values, FormatContext & fc ) const -> decltype( fc.out() )
    {
        std::ostringstream out;
        for ( size_t i = 0; i != values.size(); ++i ) {
            out << (int32_t)values[i];
            if ( i != values.size() - 1 ) {
                out << ",";
            }
        }

        return std::format_to( fc.out(), "[{}]", out.str() );
    }
};

template<> struct std::formatter<Endpoint> {
    template <typename ParseContext>
        constexpr auto parse( ParseContext & pc ) -> decltype( pc.begin() )  {
            return pc.begin();
        }

    template <typename FormatContext>
        auto format( const Endpoint & value, FormatContext & fc ) const -> decltype( fc.out() ) {
            return std::format_to( fc.out(), "'{}'::{}", value.host, value.port );
        }
};
