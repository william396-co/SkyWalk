
#ifndef __SRC_IO_HARBOR_H__
#define __SRC_IO_HARBOR_H__

#include <vector>
#include <cassert>
#include <cstdint>

#include "io.h"
#include "base/types.h"
#include "utils/types.h"
#include "base/endpoint.h"

class EmptyLock
{
public:
    EmptyLock() {}
    virtual ~EmptyLock() {}
    virtual void lock() {}
    virtual void unlock() {}
};

//
// Harbor
//
class Harbor
{
public:
    enum NodeType
    {
        NONE = 0,
        ACCEPT = 1,
        CONNECT = 2,
    };

    Harbor( bool withlock = false );
    ~Harbor();

    static HostID splice( ZoneID id, HostID hostid );
    static std::pair<ZoneID, HostID> split( HostID id );

public:
    // 添加
    bool add( NodeType ntype, HostType type,
        HostID id, sid_t sid, const Endpoint & ep );

    // 删除
    void del( sid_t sid );

    // 根据sid查找HostID
    HostID hostid( sid_t sid ) const;
    HostType hosttype( sid_t sid ) const;

    // 获取主机ID对应的会话ID
    sid_t sid( HostID id ) const;
    const Endpoint * endpoint( HostID id ) const;

    // 获取类型对应的会话ID列表
    void sids( sids_t & sids ) const;
    void sids( HostType type, sids_t & sids ) const;
    void sids( const HostTypes & types, sids_t & sids ) const;

    // 检查连接是否存在
    bool has( HostID id, HostType type ) const;

    // 合并
    bool merge( const HostIDs & settings,
        sids_t & shutdowns, HostIDs & connects );

    // 随机
    uint32_t random( HostType type ) const;
    bool random( HostType type, sid_t & sid, Endpoint & ep ) const;

private:
    struct Node
    {
        HostID id;
        sid_t sid;
        HostType type;
        NodeType ntype;
        Endpoint endpoint;

        Node()
            : Node( NONE, HostType::None, 0, 0 )
        {}

        Node( NodeType ntype_, HostType type_, HostID id_, sid_t sid_, const Endpoint & ep_ = Endpoint() )
            : id( id_ ),
              sid( sid_ ),
              type( type_ ),
              ntype( ntype_ ),
              endpoint( ep_ )
        {}
    };
    typedef UnorderedMap<HostID, Node *> NodeMap;
    typedef UnorderedMap<sid_t, HostID> SessionMap;

    bool m_Withlock;
    EmptyLock * m_Lock;
    NodeMap m_NodeMap;
    SessionMap m_SessionMap;
};

#endif
