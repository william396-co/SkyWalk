#pragma once
#include <vector>
#include <unordered_set>

#include "types.h"
#include "resource.h"
#include "utils/types.h"

typedef std::unordered_set<QuestID> QuestIDSet;
typedef std::vector<QuestID> QuestIDList;

// 任务类型
typedef int32_t QuestType; // 任务类型
enum
{
    eQuestType_None = 0,        // 非法的任务
    eQuestType_Main = 1,        // 主线任务
    eQuestType_Branch = 2,      // 支线任务
    eQuestType_Daily = 3,       // 日常任务
    eQuestType_Weekly = 4,      // 周常任务
    eQuestType_Achievement = 5, // 成就任务
    eQuestType_Guide = 6,       // 引导任务
};

enum
{
    eQuestState_None = 0,   // 未接受
    eQuestState_Accept = 1, // 已接受
    eQuestState_Done = 2,   // 已完成
    eQuestState_Submit = 3, // 已提交
};

struct BriefQuest
{
    QuestID id = 0; // 任务ID
    int64_t count = 0; // 计数

    BriefQuest() {}
    BriefQuest( QuestID id_, int64_t count_ ) : id(id_), count(count_) {}
};

struct WantedOption
{
    int32_t id = 0;     // 选项
    Resources rewards;  // 奖励

    WantedOption() = default;
    WantedOption( int32_t id_, const Resources & rewards_ ) : id(id_), rewards(rewards_) {}
    bool operator== ( int32_t id_ ) const { return id_ == id; }
};
typedef std::vector<WantedOption> WantedOptions;

