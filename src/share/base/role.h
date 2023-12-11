
#ifndef __SRC_BASE_ROLE_H__
#define __SRC_BASE_ROLE_H__

#include <algorithm>
#include <vector>
#include "types.h"

typedef std::pair<uint32_t, int64_t> AvatarPair;

// Avatar
struct AvatarInfo
{
    uint32_t spectacle = 0;  // 奇观
    std::vector<AvatarPair> avatars; // 外观
};

struct RoleState
{
    UnitID id = 0;
    uint32_t level = 0;
    std::string name;
    HostID gate_hostid = 0;
    HostID scene_hostid = 0;
    RoleState() {}
    RoleState( UnitID i, uint32_t lv, const std::string & n, HostID h1, HostID h2 )
        : id( i ), level( lv ), name( n ), gate_hostid( h1 ), scene_hostid( h2 ) {}
};
typedef std::vector<RoleState> RoleStateList;

// 角色属性
enum
{
    eRoleProp_None = 0,

    ///////////////////////////////////////////////////////////
    eRoleProp_Level = 1,            // 等级
    eRoleProp_VipLevel = 2,         // VIP等级
    eRoleProp_Battlepoint = 3,      // 战斗力
    eRoleProp_Avatar = 4,           // AVATAR
    eRoleProp_Totalpay = 5,         // 累计支付
    eRoleProp_StrengthTime = 6,     // 体力恢复时间
    eRoleProp_GlobalBits = 7,       // 全局设置表示

    ////////////////////////////已定///////////////////////////////
    eRoleProp_Money = 10001,        // 元宝
    eRoleProp_Diamond = 10002,      // 钻石(绑银)
    eRoleProp_Gamecoin = 10003,     // 金币
    eRoleProp_Exp = 10004,          // 经验
    eRoleProp_VipExp = 10005,       // VIP经验
    eRoleProp_Strength = 10006,     // 体力
    eRoleProp_Arenacoin = 10007,    // 竞技场券


    eRoleProp_Guildcoin = 10013, // 公会币
};

// 玩家状态(RoleStatusCommand)
enum RoleStatus {
    eRoleStatus_Online = 1,         // 在线
    eRoleStatus_SceneLoaded = 2,    // 场景加载完成
    eRoleStatus_SceneLogined = 3,   // 场景登录成功
    eRoleStatus_WorldLoaded = 4,    // 世界加载完成
    eRoleStatus_WorldLogined = 5,   // 世界登录完成
};

// 费用
struct Fee
{
    int32_t prop = 0;
    int64_t value = 0;

    void clear() {
        prop = 0;
        value = 0;
    }
};
typedef std::vector<Fee> FeeList;

inline void APPEND_FEELIST( FeeList & dst, const Fee & fee )
{
    if ( fee.prop == 0 || fee.value == 0 ) {
        return;
    }

    size_t i = 0;
    for ( ; i < dst.size(); ++i ) {
        if ( dst[i].prop == fee.prop ) {
            dst[i].value += fee.value;
            break;
        }
    }
    if ( i == dst.size() ) {
        dst.push_back( fee );
    }
}

// 设置字段
enum SettingsField
{
    eSettingsField_AddFriend = 1,  // 是否允许添加好友
    eSettingsField_ViewOther = 2,  // 查看其他人信息
    eSettingsField_AccessZone = 3, // 仅仅允许好友查看空间
};

struct Skill
{
    SpellID id = 0;         // 技能ID
    int32_t type = 0;       // 技能类型( 1 主动 )
    int32_t level = 0;      // 技能等级
    uint32_t building = 0;  // 所属建筑

    Skill() {}
    Skill( SpellID id_ )
        : id( id_ ), level( 1 ) {}
    Skill( SpellID id_, int32_t level_, uint32_t building_ = 0, int32_t type_ = 0 )
        : id( id_ ), type( type_ ), level( level_ ), building( building_ ) {}
};

// 技能列表
typedef std::vector<Skill> SkillList;

// 英雄状态
typedef uint8_t HeroState;
enum
{
    eHeroState_Fired = 0, // 解雇
    eHeroState_Hire = 1,  // 雇佣(未出站)
    eHeroState_Fight = 2, // 出站
};

// 阵型状态
enum
{
    eFormationState_Idle = 0,   // 未上阵
    eFormationState_Fight = 1,  // 出战中
    eFormationState_Defend = 2, // 防守中
    eFormationState_Dead = 3,   // 阵亡
};

struct FormationState
{
    uint32_t order; // 序号
    uint32_t state; // 状态

    FormationState() : order( 0 ), state( 0 ) {}
    FormationState( uint32_t o, uint32_t s ) : order( o ), state( s ) {}

    bool operator==( uint32_t o ) const {
        return order == o;
    }
};

// 阵型
typedef std::vector<HeroID> Formation;
typedef std::vector<HeroID> HeroIDList;

// 排行榜单元
struct RankRole
{
    UnitID roleid;        // 角色ID
    uint32_t rank;        // 当前排名
    uint32_t avatar;      // 头像
    std::string rolename; // 角色名
    uint32_t score;       // 积分
    uint32_t level;       // 等级
    uint32_t viplv;       // vip等级
    uint32_t lastrank;    // 上赛季排名

    RankRole()
        : roleid( 0 ),
          rank( 0 ),
          avatar( 0 ),
          score( 0 ),
          level( 0 ),
          viplv( 0 ),
          lastrank( 0 )
    {}

    bool operator==( UnitID id ) const {
        return this->roleid == id;
    }
};

typedef std::vector<RankRole> RankRoleList;
typedef RankRoleList::iterator RankRoleIter;

// 每日重置次数
struct DailyTimes
{
    uint32_t buyStrengthTimes;
    uint32_t buyGoldTimes;
    uint32_t buySklpointTimes;
    uint8_t giftStrengthDraw;        // 领取赠送体力状态

    DailyTimes()
        : buyStrengthTimes( 0 ),
          buyGoldTimes( 0 ),
          buySklpointTimes( 0 ),
          giftStrengthDraw( 0 )
    {}

    void addBuyEnergyTimes( uint32_t times = 1 ) {
        buyStrengthTimes += times;
    }

    void addBuyGoldTimes( uint32_t times = 1 ) {
        buyGoldTimes += times;
    }

    void addBuySklpointTimes( uint32_t times = 1 ) {
        buySklpointTimes += times;
    }

    void markgiftStrengthDraw( uint32_t stage ) {
        if ( isGiftStrengthDraw( stage ) ) return;
        giftStrengthDraw |= ( 1 << stage );
    }

    bool isGiftStrengthDraw( uint32_t stage ) {
        return giftStrengthDraw & ( 1 << stage );
    }

    void reset() {
        buyStrengthTimes = 0;
        buyGoldTimes = 0;
        buySklpointTimes = 0;
        giftStrengthDraw = 0;
    }
};

// 心愿单
struct GachaWishList
{
    int64_t timestamp = 0;   // 时间
    uint32_t hitontimes = 0; // 命中次数
    UIntVec wishlist;        // 心愿单
};

// 首充重置
struct PayReset
{
    uint32_t resetid = 0;
    int64_t starttime = 0;
    int64_t endtime = 0;

    PayReset() = default;
    PayReset( uint32_t _resetid, int64_t _starttime, int64_t _endtime )
        : resetid( _resetid ), starttime( _starttime ), endtime( _endtime )
    {}
};
typedef std::vector<PayReset> PayResetList;

// 充值记录
struct ChargeRecord
{
    std::string package;
    uint32_t times = 0;
    uint32_t resetid = 0;
    time_t timestamp;

    ChargeRecord() {}
    ChargeRecord( const std::string & id, uint32_t rid, time_t now )
        : package( id ), times(1), resetid( rid ), timestamp( now )
    {}

    bool operator== ( const std::string & id ) const { return id == package; }
};
typedef std::vector<ChargeRecord> ChargeRecordList;

#endif
