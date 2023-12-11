
#ifndef __SRC_BASE_MODULES_H__
#define __SRC_BASE_MODULES_H__

#include "types.h"

enum {
    eSystemStatus_None = 0,     // 非法
    eSystemStatus_Prepare = 1,  // 准备
    eSystemStatus_Running = 2,  // 开始
    eSystemStatus_Settle = 3,   // 结算
    eSystemStatus_Complete = 4, // 完成
};

// 系统ID
enum
{
    ////////////////////////////////////////////////////////////////////////////
    // 系统
    ////////////////////////////////////////////////////////////////////////////

    eSystemID_Invalid = 0,      // 非法系统ID
    eSystemID_Planet = 1,       // 建造
    eSystemID_Dungeon = 2,      // PVE系统
    eSystemID_Gacha = 3,        // 扭蛋
    eSystemID_Payment = 4,      // 充值
    eSystemID_Hangup = 5,       // 挂机功能
    eSystemID_Mail = 6,         // 邮件
    eSystemID_Quest = 7,        // 主线任务
    eSystemID_Hero = 8,         // 英雄
    eSystemID_Arena = 9,        // 竞技场

    eSystemID_Fetter = 11,      // 羁绊功能
    eSystemID_Population = 12,  // 造物
    eSystemID_Pray = 13,        // 祈愿
    eSystemID_Event = 14,       // 事件
    eSystemID_WorldBoss = 15,   // 世界BOSS
    eSystemID_DailyQuest = 16,  // 日常任务
    eSystemID_WeeklyQuest = 17, // 周常任务
    eSystemID_Achievement = 18, // 成就

    ////////////////////////////////////////////////////////////////////////////
    // 活动
    ////////////////////////////////////////////////////////////////////////////

    // NOTICE: 从1000开始把。。。。。。
};

// 游戏模块定义
enum {
    eModulePoint_Invalid = 0,                    // 非法模块类型
    eModulePoint_GM = 1,                         // GM命令
    eModulePoint_DBException = 2,                // 数据异常
    eModulePoint_Newborn = 3,                    // 创角

    eModulePoint_ItemUse = 10,                   // 使用道具
    eModulePoint_MailAttachment = 11,            // 领取邮件
    eModulePoint_Payment = 12,                   // 支付
    eModulePoint_PayRefund = 13,                 // 退款
    eModulePoint_HeroLevelup = 14,               // 卡牌升级
    eModulePoint_HeroBreakThrough = 15,          // 卡牌突破
    eModulePoint_DrawChapterRewards = 16,        // 领取章节奖励
    eModulePoint_SettlePVEDungeon = 17,          // PVE主线关卡结算
    eModulePoint_BuyArenaTickets = 18,           // 购买竞技场券
    eModulePoint_DrawArenaRankReward = 19,       // 领取竞技场每日排行奖励
    eModulePoint_ArenaChallenge = 20,            // 竞技场挑战
    eModulePoint_ArenaSettle = 21,               // 竞技场结算
    eModulePoint_DrawArenaHighLightsReward = 22, // 高光奖励
};

#endif
