#pragma once

#include <unordered_map>
#include <vector>

#include "base/types.h"
#include "domain/rtstate.h"
#include "utils/ipcs.h"
#include "utils/types.h"

class RouterMeta;
struct ChannelInfo
{
    SceneID sceneid = 0;
    HostID hostid = 0;
    int32_t count = 0;

    ChannelInfo() = default;
    ChannelInfo( SceneID id, HostID hid, int32_t count = 0 )
        : sceneid( id ), hostid( hid ), count( count )
    {
    }
};
using ChannelList = std::vector<ChannelInfo>;

class ChannelRouter
{
public:
    ChannelRouter( size_t count = 0 );
    ~ChannelRouter();

public:
    HostID get( SceneID sceneid ) const;
    HostID select( const HostIDs & ids ) const;
    void remove( SceneID id );
    void fill( std::vector<ChannelInfo> & tablelist ) const;
    // count == -1: 自增
    void update( SceneID id, HostID hostid, int32_t count = -1 );

    void unlink( bool removeflag ) { m_RTState.unlink( removeflag ); }

private:
    RTState<SceneID, RouterMeta> m_RTState;             // 临时存储
    std::unordered_map<HostID, int32_t> m_CopyMap;
    std::unordered_map<SceneID, ChannelInfo> m_ChannelMap;
};
