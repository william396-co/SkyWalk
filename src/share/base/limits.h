#pragma once

#include <climits>

//==============================================================================
// 最大值限制
//==============================================================================

enum
{
#if defined( __DEBUG__ )
    eMaxClientsCount = 20000,              // 服务器最大容量
    eMaxRecvPacketsRate = 50,              // 服务器收包率
    eMaxAccountIDLen = 32,                 // 帐户长度
    eMaxRolenameLength = 64,               // 角色名最大长度
    eMaxChatInterval = 20,                 // 角色聊天时间间隔
    eMaxRankListCapacity = 256,            // 排行榜最大容量
    eMaxFightHeroCount = 5,                // 最大出站英雄
    eMaxFormationSlotCount = 9,            // 阵型最大位置数
    eMaxArenaPKRecords = 10,               // 竞技场最大对战记录数
    eMaxMailBoxQuota = 256,                // 邮箱容量限制
    eMaxMatchMakerList = 2,                // 最大匹配列表
    eMaxWorldBossDamageRank = 10,          // 世界BOSS伤害榜单长度
    eMaxSessionIdleTime = 600,             // 断线后会话保留时间(单位:秒)
    eMaxVerifyPoolCapacity = 2048,         // 验证池容量
    eMaxRecentLoginRoleCount = 512,        // 最近登录的最大角色数
    eMaxRefreshRelationStatusSeconds = 60, // 更新好友状态时间限制
    eMaxSendOfflineInvitationSeconds = 5,  // 发送离线邀请的时间限制
    eMaxCheckBulletinInterval = 60,        // 检查公告时间间隔(单位:秒)
    eMaxChatContentLength = 128,           // 聊天内容长度限制(32汉字)
    eMaxMediaCenterCapacity = 200,         // 媒体中心容量
    eMaxOfflineMessageCount = 100,         // 最大的离线消息数
    eMaxQueryTimeoutSeconds = 32,          // 数据库查询超时时间
    eMaxGuildnameLength = 32,              // 公会名最大长度
    eMaxDungeonGrade = 7,                  // 最大评级
    eMaxZoneLikeInterval = 5,              // 分享时间间隔
    eMaxAttachmentsSize = 100,             // 邮件附件最大个数
    eMaxBattleLogLength = 10485760,        // 战斗日志长度
    eMaxFormationOrderNumber = 50,         // 单系统的最大阵容数量
    eMaxErrorMarksLifetime = 1800,         // 错误掩码的生命周期
    eMaxGuildwarRankCount = 512,           // 排行个数
    eMaxSessionDelaySeconds = 5000,        // 客户端最大延迟5秒
    eMaxBossDamageRankCount = 200,         // 世界BOSS伤害排行个数
    eMaxFightDungeonList = 5,              // 最多同时进行的挂机副本数量
    eMaxWaitAssignChannelSeconds = 5,      // 分配线路最大等待时间
    eMaxSkillLevel = 10,                   // 技能最大等级
    eMaxBattleFrame = 7200,                // 每场战斗最大7200帧
#else
    eMaxClientsCount = 20000,              // 服务器最大容量
    eMaxRecvPacketsRate = 100,             // 服务器收包率
    eMaxAccountIDLen = 32,                 // 帐户长度
    eMaxRolenameLength = 64,               // 角色名最大长度
    eMaxChatInterval = 20,                 // 角色聊天时间间隔
    eMaxRankListCapacity = 256,            // 排行榜最大容量
    eMaxFightHeroCount = 5,                // 最大出站英雄
    eMaxFormationSlotCount = 9,            // 阵型最大位置数
    eMaxArenaPKRecords = 10,               // 竞技场最大对战记录数
    eMaxMailBoxQuota = 256,                // 邮箱容量限制
    eMaxMatchMakerList = 2,                // 最大匹配列表
    eMaxWorldBossDamageRank = 10,          // 世界BOSS伤害榜单长度
    eMaxSessionIdleTime = 600,             // 断线后会话保留时间(单位:秒)
    eMaxVerifyPoolCapacity = 2048,         // 验证池容量
    eMaxRecentLoginRoleCount = 2048,       // 最近登录的最大角色数
    eMaxRefreshRelationStatusSeconds = 60, // 更新好友状态时间限制
    eMaxSendOfflineInvitationSeconds = 5,  // 发送离线邀请的时间限制
    eMaxCheckBulletinInterval = 60,        // 检查公告时间间隔(单位:秒)
    eMaxChatContentLength = 128,           // 聊天内容长度限制(32汉字)
    eMaxMediaCenterCapacity = 20000,       // 媒体中心容量
    eMaxOfflineMessageCount = 100,         // 最大的离线消息数
    eMaxQueryTimeoutSeconds = 32,          // 数据库查询超时时间
    eMaxGuildnameLength = 32,              // 公会名最大长度
    eMaxDungeonGrade = 7,                  // 最大评级
    eMaxZoneLikeInterval = 5,              // 分享时间间隔
    eMaxAttachmentsSize = 100,             // 邮件附件最大个数
    eMaxBattleLogLength = 10485760,        // 战斗日志长度
    eMaxFormationOrderNumber = 50,         // 单系统的最大阵容数量
    eMaxErrorMarksLifetime = 1800,         // 错误掩码的生命周期
    eMaxGuildwarRankCount = 512,           // 排行个数
    eMaxSessionDelaySeconds = 5000,        // 客户端最大延迟5秒
    eMaxBossDamageRankCount = 200,         // 世界BOSS伤害排行个数
    eMaxFightDungeonList = 5,              // 最多同时进行的挂机副本数量
    eMaxWaitAssignChannelSeconds = 20,     // 分配线路最大等待时间
    eMaxSkillLevel = 10,                   // 技能最大等级
    eMaxBattleFrame = 7200,                // 每场战斗最大7200帧
#endif
};
