
#ifndef __SRC_EXTRA_HARBORTRACER_H__
#define __SRC_EXTRA_HARBORTRACER_H__

#include "io/io.h"
#include "io/harbor.h"
#include "utils/types.h"
#include "base/types.h"
#include "base/endpoint.h"

//
// 分布追踪技术
//
// 监控分布式架构的组网
// 数据加载成功后的追踪
//
class HarborTracer
{
public :
    HarborTracer( Harbor * h )
        : m_Harbor(h), m_UniqueID(0) {}
    ~HarborTracer() = default;
    typedef std::function< void ( HostType, sid_t ) > Handler;

public :
    // 监控架构的组网
    uint32_t watch( HostType type, const Handler & handler ) {
        uint32_t id = ++m_UniqueID;
        m_HandlerMap.emplace( id, std::pair<HostType, Handler> { type, handler } );
        auto pos = m_WatcherMap.find( type );
        if ( pos != m_WatcherMap.end() ) {
            pos->second.emplace_back( handler, id );
        } else {
            m_WatcherMap.emplace( type, std::vector<std::pair<Handler,uint32_t>>{ { handler, id } } );
        }
        return id;
    }

    // 数据加载完成后的回调
    void complete( uint32_t id ) {
        auto pos = m_HandlerMap.find( id );
        if ( pos != m_HandlerMap.end() ) {
            sids_t sids;
            m_Harbor->sids( pos->second.first, sids );
            std::for_each( sids.begin(), sids.end(), [&]( sid_t sid ) {
                pos->second.second( pos->second.first, sid );
            } );
            m_HandlerMap.erase( pos );
        }
    }

    // 分发组网完成后的事件
    void dispatch( HostType type, sid_t sid ) {
        auto pos = m_WatcherMap.find( type );
        if ( pos != m_WatcherMap.end() ) {
            std::for_each( pos->second.begin(), pos->second.end(), [&]( auto && kv ) {
                if ( m_HandlerMap.end()
                        == m_HandlerMap.find( kv.second ) ) {
                    kv.first( type, sid );
                }
            } );
        }
    }

private :
    Harbor * m_Harbor;
    uint32_t m_UniqueID;
    UnorderedMap<uint32_t, std::pair<HostType, Handler>> m_HandlerMap;
    UnorderedMap<HostType, std::vector<std::pair<Handler, uint32_t>>> m_WatcherMap;
};

#endif
