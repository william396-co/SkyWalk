#ifndef __SRC_BASE_OPERATION_H__
#define __SRC_BASE_OPERATION_H__

#include <stdint.h>
#include <vector>

#include "types.h"

// 迷宫城活动格子类型
enum
{
    eOpSlotType_None = 0,     // 非法
    eOpSlotType_Startup = 1,  // 起点
    eOpSlotType_Boss = 2,     // BOSS
    eOpSlotType_Blank = 3,    // 空格
    eOpSlotType_Block = 4,    // 阻挡
    eOpSlotType_Monster = 5,  // 小怪
    eOpSlotType_Rewards = 6,  // 宝箱奖励
    eOpSlotType_Article = 7,  // 道具
    eOpSlotType_Key = 8,      // 钥匙格子
    eOpSlotType_Terminal = 9, // 终点
};

// 道具类型
enum
{
    eOpArticleType_None = 0,
    eOpArticleType_Light = 1, // 烛火明灯
    eOpArticleType_TNT = 2,   // 炸弹
};

// 活动时间段
struct TimeSection
{
    std::string prepare; // 准备时间
    std::string start;   // 开始时间
    std::string finish;  // 结束时间
};

struct ActivityStage
{
    int64_t timestamp;
    uint32_t count;
};
typedef std::vector<ActivityStage> ActivityStages;

struct OpSlotElement
{
    uint32_t type;
    uint32_t elementid;
    OpSlotElement() : type( 0 ), elementid( 0 ) {}
    OpSlotElement( uint32_t t ) : type( t ), elementid( 0 ) {}
    OpSlotElement( uint32_t t, uint32_t d ) : type( t ), elementid( d ) {}
};

// 运营活动阶段信息
struct StageRecord
{
    int32_t day = 0;    // 天数
    int64_t score = 0;  // 当天的积分
    IntVec statuslist; // 领取记录

    bool operator== ( int32_t d ) const { return d == day; }
    StageRecord() {}
    StageRecord( int32_t d, int64_t s, const IntVec & res ) : day(d), score(s), statuslist(res) {}
};
typedef std::vector<StageRecord> StageRecords;

struct TimePeriod
{
    Period absolute;    // 绝对时间
    Period startzone;   // 开服天数
    Period createrole;  // 创角天数
};

#endif
