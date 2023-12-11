
#ifndef __SRC_BASE_RESOURCE_H__
#define __SRC_BASE_RESOURCE_H__

#include "types.h"
#include "role.h"
#include "item.h"

// 资源定义
typedef int32_t ResourceType; // 资源类型
enum
{
    eResourceType_None = 0,     // 非法资源
    eResourceType_Item = 1,     // 物品
    eResourceType_Pack = 2,     // 掉落包
    eResourceType_Hero = 3,     // 英雄
    eResourceType_Props = 4,    // 属性
    eResourceType_Effect = 5,   // 效果类资源
};

struct Resource
{
    ResourceType type = 0; // 类型
    int32_t object = 0;    // 对象ID
    int64_t count = 0;     // 对象数量
    int32_t param1 = -1;   // 参数1
    int32_t param2 = -1;   // 参数2

    Resource() = default;

    void clear() {
        type = 0;
        object = 0;
        count = 0;
        param1 = -1;
        param2 = -1;
    }

    template<typename K, typename V>
    Resource( const std::tuple<K, V> & value )
        : type( eResourceType_Item ),
          object( std::get<0>(value) ), count( std::get<1>(value) ) {}

    Resource( ResourceType type_, int32_t object_, int64_t count_ )
        : type( type_ ), object( object_ ), count( count_ ) {}
    Resource( ResourceType type_, int32_t object_, int64_t count_, int32_t param1_ )
        : type( type_ ), object( object_ ),
          count( count_ ), param1( param1_ ), param2( -1 ) {}
    Resource( ResourceType type_, int32_t object_, int64_t count_, int32_t param1_, int32_t param2_ )
        : type( type_ ), object( object_ ),
          count( count_ ), param1( param1_ ), param2( param2_ ) {}
};
typedef std::vector<Resource> Resources; // 资源列表

// 掉落几率
struct RandomDrop
{
    ResourceType type; // 类型
    uint32_t object;   // 对象ID
    int32_t count;     // 对象数量
    uint32_t prob;     // 掉落几率

    RandomDrop()
        : type( eResourceType_None ),
          object( 0 ),
          count( 0 ),
          prob( 0 )
    {}

    RandomDrop( ResourceType _type, uint32_t _object, int32_t _count, uint32_t _prob )
        : type( _type ),
          object( _object ),
          count( _count ),
          prob( _prob )
    {}
};
typedef std::vector<RandomDrop> RandomDropList; // 随机掉落列表

inline bool CHECK_RESOURCES( Resources & resources )
{
    Resources::iterator it;

    for ( it = resources.begin(); it != resources.end(); ) {
        if ( it->count > 0 ) {
            ++it;
        } else {
            it = resources.erase( it );
        }
    }

    return !resources.empty();
}

inline void APPEND_RESOURCES( const Resource & from, Resources & to )
{
    size_t i = 0;

    for ( i = 0; i < to.size(); ++i ) {
        if ( from.type == to[i].type
            && from.object == to[i].object
            && from.param1 == to[i].param1 && from.param2 == to[i].param2 ) {
            to[i].count += from.count;
            break;
        }
    }

    if ( i == to.size() ) { to.push_back( from ); }
}

inline void MERGE_RESOURCES( const Resources & from, Resources & to )
{
    Resources::const_iterator it;

    for ( it = from.begin(); it != from.end(); ++it ) {
        if ( it->type == 0
            || it->count == 0
            || it->object == 0 ) {
            continue;
        }

        APPEND_RESOURCES( *it, to );
    }
}

inline Resources MULTIPLE_RESOURCES( const Resources & rewards, uint32_t multiple )
{
    Resources f_rewards;

    for ( size_t i = 0; i < rewards.size(); ++i ) {
        int64_t count = rewards[i].count;

        if ( multiple > 1 ) { count *= multiple; }
        f_rewards.push_back(
            Resource( rewards[i].type, rewards[i].object, count, rewards[i].param1, rewards[i].param2 ) );
    }

    return f_rewards;
}

inline Resources PERCENT_RESOURCES( const Resources & rewards, uint32_t percent )
{
    Resources f_rewards;

    if ( percent == PERCENT_BASE_VALUE ) {
        return rewards;
    }

    for ( size_t i = 0; i < rewards.size(); ++i ) {
        int64_t count = rewards[i].count;
        count = count * PERCENT( percent );
        if ( count > 0 ) {
            f_rewards.push_back(
                    Resource( rewards[i].type, rewards[i].object, count, rewards[i].param1, rewards[i].param2 ) );
        }
    }

    return f_rewards;
}

inline Resources PERCENT_PLUS_RESOURCES( const Resources & rewards, uint32_t percent )
{
    Resources f_rewards;

    if ( percent == 0 ) {
        return rewards;
    }

    for ( size_t i = 0; i < rewards.size(); ++i ) {
        int64_t count = rewards[i].count;
        count += count * PERCENT( percent );
        f_rewards.push_back(
            Resource( rewards[i].type, rewards[i].object, count, rewards[i].param1, rewards[i].param2 ) );
    }

    return f_rewards;
}

#endif
