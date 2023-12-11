
#ifndef __SRC_BASE_EVENT_H__
#define __SRC_BASE_EVENT_H__

#include "types.h"

typedef int32_t EventType;                 // 事件类型
typedef std::vector<EventType> EventTypes; // 事件类型列表

struct EventArg
{
    EventType type = 0;
    int64_t param1 = 0; // 参数1
    int64_t param2 = 0; // 参数2
    int64_t param3 = 0; // 参数3
    EventArg() {}
    EventArg( EventType t, int64_t p1, int64_t p2, int64_t p3 ) : type(t), param1(p1), param2(p2), param3(p3) {}
};

//
// 事件类型
//

enum
{
    eEventType_Invalid = 0,               // 非法事件类型
    eEventType_FinishCopy = 1,            // 通关XX副本XX次
    eEventType_KillMonster = 2,           // 击杀XX怪
    eEventType_CollectItem = 3,           // 收集道具
    eEventType_TalkNpc = 4,               // 与NPC对话
    eEventType_DailyLogin = 5,            // 每日登录XX次
    eEventType_ChallengeCopy = 6,         // 挑战副本

    // TODO:::: 继续增加 继续增加 继续增加 继续增加
};

#endif
