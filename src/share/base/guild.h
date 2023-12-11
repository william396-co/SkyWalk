
#ifndef __SRC_BASE_GUILD_H__
#define __SRC_BASE_GUILD_H__

#include "types.h"

typedef std::vector<GuildID> GuildIDList;

// 公会属性
enum
{
    eGuildProp_Funds = 1,    // 建设资金
    eGuildProp_Actives = 2,  // 活跃度
    eGuildProp_Level = 3,    // 公会等级
    eGuildProp_Practice = 4, // 修业度
};

// 成员类型
enum
{
    eMemberType_None = 0,      // 非法成员
    eMemberType_Owner = 1,     // 会长
    eMemberType_ViceOwner = 2, // 副会长
    eMemberType_Member = 4,    // 普通成员

    eMemberType_All = eMemberType_Owner | eMemberType_ViceOwner | eMemberType_Member,
};

// 公会验证方式
enum
{
    eVerifyMode_None = 0, // 非法验证方式
    eVerifyMode_Need = 1, // 需要验证
    eVerifyMode_Free = 2, // 自由加入
};

// 公会战双方
enum
{
    eGuildSide_None = 0, // 非法
    eGuildSide_Red = 1,  // 红方
    eGuildSide_Blue = 2, // 蓝方
};

// 公会积分
struct GuildScore
{
    GuildScore()
        : level( 0 ), funds( 0 ) {}
    GuildScore( uint32_t l, uint32_t f )
        : level( l ), funds( f ) {}

    bool operator>( const GuildScore & s ) const
    {
        if ( s.level == level ) {
            return funds > s.funds;
        }

        return level > s.level;
    }

    bool operator<( const GuildScore & s ) const
    {
        if ( level == s.level ) {
            return funds < s.funds;
        }

        return level < s.level;
    }

    bool operator==( const GuildScore & s ) const
    {
        return level == s.level && funds == s.funds;
    }

    uint32_t level; // 等级
    uint32_t funds; // 基金
};

// 公会日常
struct GuildRoleDaily
{
    uint32_t status;  // 领取状态
    uint32_t revives; // 复活次数

    GuildRoleDaily()
        : status( 0 ),
          revives( 0 )
    {}

    void clear()
    {
        status = 0;
        revives = 0;
    }
};

// first blood  第一滴血
// double kill  双杀
// triple kill  三杀
// quadra kill  四杀
// penta kill   五杀
// ace          团灭
// unstoppable  势不可挡
// godlike      横扫千军
// legendary    超神

// 公会战统计
struct GuildwarStat
{
    int32_t revive;        // 复活次数
    int32_t killcount;     // 杀敌数
    int32_t doublekill;    // 双杀
    int32_t triplekill;    // 三杀
    int32_t legendarykill; // 无双杀敌数

    GuildwarStat()
        : revive( 0 ),
          killcount( 0 ),
          doublekill( 0 ),
          triplekill( 0 ),
          legendarykill( 0 )
    {}

    bool operator>( const GuildwarStat & s ) const
    {
        if ( s.killcount == killcount ) {
            if ( s.legendarykill == legendarykill ) {
                if ( s.triplekill == triplekill ) {
                    return doublekill > s.doublekill;
                }

                return triplekill > s.triplekill;
            }

            return legendarykill > s.legendarykill;
        }

        return killcount > s.killcount;
    }

    bool operator<( const GuildwarStat & s ) const
    {
        if ( s.killcount == killcount ) {
            if ( s.legendarykill == legendarykill ) {
                if ( s.triplekill == triplekill ) {
                    return doublekill < s.doublekill;
                }

                return triplekill < s.triplekill;
            }

            return legendarykill < s.legendarykill;
        }

        return killcount < s.killcount;
    }

    bool operator==( const GuildwarStat & s ) const
    {
        return s.killcount == killcount
            && s.legendarykill == legendarykill
            && s.triplekill == triplekill && s.doublekill == doublekill;
    }

    GuildwarStat & operator+=( const GuildwarStat & addon )
    {
        revive += addon.revive;
        killcount += addon.killcount;
        doublekill += addon.doublekill;
        triplekill += addon.triplekill;
        legendarykill += addon.legendarykill;
        return *this;
    }
};

struct GuildwarStatBrief
{
    uint32_t score;
    uint32_t members;
    uint32_t teams;
    uint32_t revives;
    uint32_t kills;

    GuildwarStatBrief()
        : score( 0 ),
          members( 0 ),
          teams( 0 ),
          revives( 0 ),
          kills( 0 )
    {}
};

#endif
