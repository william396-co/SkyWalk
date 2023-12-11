
#ifndef __SRC_BASE_DUNGEON_H__
#define __SRC_BASE_DUNGEON_H__

#include <algorithm>
#include <utility>
#include <vector>

#include "types.h"

// 关卡类型
enum
{
    eDungeonType_Main = 1,          // 主线关卡
    eDungeonType_Arena = 2,         // 竞技场关卡
    eDungeonType_Gold = 3,          // 金币关卡
    eDungeonType_Exp = 4,           // 经验关卡
    eDungeonType_Equip = 5,         // 装备关卡
    eDungeonType_NormalTower = 6,   // 普通塔关卡
    eDungeonType_TimeTower = 7,     // 时光塔关卡
    eDungeonType_Newbie = 8,        // 新手关卡
    // TODO: 继续添加

    eDungeonType_Free = 99,         // 自由关卡
};

inline std::string DUNGEON_DESCRIBE( int32_t type ) {
    std::string desc;
    switch ( type ) {
        case eDungeonType_Main: desc = "Main"; break;
        case eDungeonType_Arena: desc = "Arena"; break;
        case eDungeonType_Gold: desc = "Gold"; break;
        case eDungeonType_Exp: desc = "Exp"; break;
        case eDungeonType_Equip: desc = "Equip"; break;
        case eDungeonType_NormalTower: desc = "NormalTower"; break;
        case eDungeonType_TimeTower: desc = "TimeTower"; break;
        case eDungeonType_Newbie: desc = "Newbie"; break;
    }
    return desc;
}

// 战斗记录类型
enum
{
    eDungeonRecord_Duration = 1,        // 通关时间
    eDungeonRecord_Score = 2,           // 积分
    eDungeonRecord_Rank = 3,            // 排名
};

// 关卡记录数据
struct DungeonRecord
{
    int32_t type = 0;  // 类型
    int64_t value = 0; // 数值
    int32_t side = 0;  // 战斗方

    DungeonRecord() = default;
    DungeonRecord( int32_t t, int64_t v, int32_t s = 1 )
        : type(t), value(v), side(s)
    {}
};
typedef std::vector<DungeonRecord> DungeonRecords;

struct ChannelInfo
{
    SceneID sceneid = 0;
    HostID hostid = 0;
    int32_t count = 0;

    ChannelInfo() = default;
    ChannelInfo( SceneID id, HostID hid, int32_t count = 0 )
        : sceneid( id ), hostid( hid ), count( count )
    {}
};
typedef std::vector<ChannelInfo> ChannelList;

// 竞技高光时刻
struct ArenaHighLight {
    int32_t type = 0;             // 类型
    UnitID roleid = 0;            // 角色
    int64_t value = 0;            // 数值
    ArenaHighLight() = default;
    ArenaHighLight( int32_t t, UnitID i, int64_t v )
        : type(t), roleid( i ), value( v ) {}
    bool operator== ( int32_t ty ) const { return type == ty; }
};

struct ArenaState {
    int32_t season = 0;    // 赛季
    int64_t timestamp = 0; // 初始化时间
    int32_t giverank = 0;  // 结算排行(结算过程中)
    int32_t cleanrank = 0; // 清空排行
    int32_t robotcount = 0;// 机器人个数
    std::vector<ArenaHighLight> highlights; // 高光时刻

    void reset( int32_t s ) {
        season = s;
        giverank = 0;
        cleanrank = 0;
    }
};

struct ArenaNotes {
    int32_t season = 0;    // 赛季
    UnitIDList board;      // 上赛季排行榜(50名)

    void reset( int32_t s ) {
        season = s;
        board.clear();
    }

    bool has( UnitID id ) const {
        return board.end() != std::find( board.begin(), board.end(), id );
    }
};

struct ArenaDaily {
    int32_t times = 0;  // 每日刷新次数

    void clear() {
        times = 0;
    }
};

struct CombatJournal {
    CombatID id = 0;        // 战斗ID
    UnitID roleid = 0;      // 自己ID
    UnitID opponent = 0;    // 对手ID
    int32_t result = 0;     // 结果, 1-胜利,2-失败, 参考CombatResult
    int32_t robotid = 0;    // 机器人ID
    uint32_t avatar = 0;    // 对手头像
    std::string rolename;   // 对手角色名
    int32_t level = 1;      // 玩家等级
    int32_t viplevel = 0;   // vip等级
    int64_t change = 0;     // 积分变化
    int64_t timestamp = 0;  // 时间戳
    std::string statistics; // 数据统计
    std::string extrainfo;  // 额外信息
};

#endif
