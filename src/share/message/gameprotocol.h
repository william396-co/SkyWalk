
#ifndef __SRC_MESSAGE_GAMEPROTOCOL_H__
#define __SRC_MESSAGE_GAMEPROTOCOL_H__

#include <map>

#include "base/battle.h"
#include "base/database.h"
#include "base/resource.h"
#include "base/role.h"
#include "base/hero.h"
#include "base/modules.h"
#include "base/guild.h"
#include "base/quest.h"
#include "base/event.h"
#include "base/dungeon.h"
#include "domain/event.h"

#include "protocol.h"

enum {
    eSSCommand_SwitchScene = 0x0301,        // 切换场景
    eSSCommand_RoleNewborn = 0x0302,        // 创角
    eSSCommand_SendMail = 0x0303,           // 发送邮件
    eSSCommand_RegisterZone = 0x0304,       // 注册区服
    eSSCommand_CharBrief = 0x0305,          // 玩家简要信息
    eSSCommand_UpdateRank = 0x0306,         // 更新排行榜
    eSSCommand_SyncRoleProps = 0x0307,      // 同步角色属性
    eSSCommand_ChangeRoleProps = 0x0308,    // 修改角色属性
    eSSCommand_ChangeRolename = 0x0309,     // 改名通知
    eSSCommand_DispatchEvent = 0x030A,      // 触发事件
    eSSCommand_SubscribeQuest = 0x030B,     // 订阅任务
    eSSCommand_OpenSystemList = 0x030C,     // 系统开放列表
    eSSCommand_DataException = 0x030D,      // 数据异常
    eSSCommand_Pay = 0x030E,                // 支付
    eSSCommand_PayCallback = 0x030F,        // 支付回调
    eSSCommand_PayRefund = 0x0310,          // 支付退款
    eSSCommand_PayRefundCallback = 0x0311,  // 支付退款回调
    eSSCommand_SyncBilling = 0x0312,        // 同步账单
    eSSCommand_SyncMainProgress = 0x0313,   // 同步主体进度
    // TODO: 0x0314-0x031F
    //
    eSSCommand_CreateArea = 0x0320,         // 创建中转区域
    eSSCommand_ResultReport = 0x0321,       // 上报战斗结果
    eSSCommand_ChatInWorld = 0x0322,        // 世界频道聊天
    eSSCommand_SyncAttribute = 0x0323,      // 同步战斗属性
    eSSCommand_QueryAttribute = 0x0324,     // 查询战斗属性
    eSSCommand_RemoveCombatVideo = 0x0325,  // 删除战斗回放
    eSSCommand_ArenaChallenge = 0x0326,     // 竞技场挑战
    eSSCommand_ArenaInfo = 0x0327,          // 竞技场信息
    eSSCommand_ArenaRankReward = 0x0328,    // 竞技场排名奖励
    eSSCommand_ArenaSettle = 0x0329,        // 竞技场单场结算
    eSSCommand_ArenaHighLightsReward = 0x032A, // 竞技场高光奖励
};

// 切换场景
MESSAGE_BEGIN( SwitchSceneCommand, eSSCommand_SwitchScene, 1024 )
    LoginMode method = LoginMode::None; // 方式
    UnitID roleid = 0;                  // 角色ID
    std::string account;                // 账号
    HostID gateid = 0;                  // 网关服务编号
    HostID sceneid = 0;                 // 场景服务编号
    int32_t level = 0;                  // 玩家等级
    int32_t friendcount = 0;            // 好友个数
    std::string version;                // 客户端版本号
    std::string deviceid;               // 客户端设备号
    std::string distinctid;             // 游客ID
    Endpoint endpoint;                  // IP和端口号
    std::string extrainfo;              // 额外信息
MESSAGE_END( SwitchSceneCommand, method, roleid, account, gateid, sceneid, level, friendcount, version, deviceid, distinctid, endpoint, extrainfo )

// 创角
MESSAGE_BEGIN( RoleNewbornCommand, eSSCommand_RoleNewborn, 1024 )
    UnitID roleid = 0;       // 角色ID
    ZoneID zoneid = 0;       // 区服ID
    std::string account;     // 账号
    std::string rolename;    // 角色名
    int64_t newborntime = 0; // 出生时间
    std::string distinctid;  // 游客ID
    std::string device;      // 客户端设备号
    std::string version;     // 客户端版本号
    Endpoint endpoint;       // IP和端口号
MESSAGE_END( RoleNewbornCommand, roleid, zoneid, account, rolename, newborntime, distinctid, device, version, endpoint )

// 发送邮件
MESSAGE_BEGIN( SendMailCommand, eSSCommand_SendMail, 2048 )
    MailID mailid = 0;                    // 邮件ID, 当邮件ID为0时, 接收方需要创建一封邮件
    MailType type = MailType::None;       // 邮件类型
    int32_t templateid = 0;               // 模板id
    int32_t tag = 0;                      // 标签
    int32_t keepdays = 0;                 // 保留天数
    MailStatus status = MailStatus::None; // 邮件状态
    UnitID receiver = eInvalidUnitID;     // 接收者
    UnitID sender = eInvalidUnitID;       // 发送者
    std::string sendername;               // 发送者名称
    time_t sendtime = 0;                  // 发送方时间
    std::string title;                    // 标题
    std::string content;                  // 内容
    std::string paramlist;                // 参数列表
    Resources attachments;                // 附件
MESSAGE_END( SendMailCommand, mailid, type, templateid, tag, keepdays, status, receiver, sender, sendername, sendtime, title, content, paramlist, attachments )

// 区服注册
MESSAGE_BEGIN( RegisterZoneCommand, eSSCommand_RegisterZone, 128 )
    ZoneID zoneid = eInvalidZoneID; // 区服ID
    uint32_t groupid = 0;           // 分组ID
    ZoneIDList mergelist;           // 合服列表
MESSAGE_END( RegisterZoneCommand, zoneid, groupid, mergelist )

// 玩家简要信息
MESSAGE_BEGIN( CharBriefCommand, eSSCommand_CharBrief, 128 )
    HostID hostid = 0;   // 主机ID
    UnitID roleid = eInvalidUnitID; // 角色id
    std::string account; // 账号
    uint32_t lv = 0;     // 玩家等级
    uint32_t viplv = 0;  // VIP等级
    uint32_t avatar = 0; // avatar
    std::string name;    // 角色名
MESSAGE_END( CharBriefCommand, hostid, roleid, account, lv, viplv, avatar, name )

// 更新排行榜
MESSAGE_BEGIN( UpdateRankCommand, eSSCommand_UpdateRank, 64 )
    TransID transid = 0;
    UnitID roleid = 0;
    int32_t ranktype = 0;
    int64_t score = 0;
MESSAGE_END( UpdateRankCommand, transid, roleid, ranktype, score )

// 修改角色属性
MESSAGE_BEGIN( ChangeRolePropsCommand, eSSCommand_ChangeRoleProps, 64 )
    UnitID roleid = eInvalidUnitID;
    int32_t module = 0;
    int32_t props = 0;
    int64_t value = 0;
MESSAGE_END( ChangeRolePropsCommand, roleid, module, props, value )

// 同步角色属性
MESSAGE_BEGIN( SyncRolePropsCommand, eSSCommand_SyncRoleProps, 128 )
    UnitID roleid = 0;
    int32_t prop = 0;
    int64_t value = 0;
    int32_t system = 0;
MESSAGE_END( SyncRolePropsCommand, roleid, prop, value, system )

// 改名通知
MESSAGE_BEGIN( ChangeRolenameCommand, eSSCommand_ChangeRolename, 1024 )
    UnitID roleid = 0;
    HostID hostid = 0;
    int32_t result = 0;
    ItemID cardid = 0;
    std::string rolename;
MESSAGE_END( ChangeRolenameCommand, roleid, hostid, result, cardid, rolename )

// 通知事件发生
MESSAGE_BEGIN( DispatchEventCommand, eSSCommand_DispatchEvent, 1024 )
    UnitID roleid = 0;
    std::vector<EventArg> events;
MESSAGE_END( DispatchEventCommand, roleid, events )

// 订阅/取消单一任务
MESSAGE_BEGIN( SubscribeQuestCommand, eSSCommand_SubscribeQuest, 1024 )
    UnitID roleid = 0;
    SceneID sceneid = 0;   // 当前所在副本
    uint8_t method = 0;    // 0-取消;1-订阅
    EventTypes typelist;    // 任务类型
    QuestIDList questlist; // 任务ID列表
MESSAGE_END( SubscribeQuestCommand, roleid, sceneid, method, typelist, questlist )

// 系统开放同步
MESSAGE_BEGIN( OpenSystemListCommand, eSSCommand_OpenSystemList, 2048 )
    uint8_t method = 0;
    UnitID roleid = 0;
    std::vector<SystemID> systemlist;
MESSAGE_END( OpenSystemListCommand, method, roleid, systemlist )

// 数据异常
MESSAGE_BEGIN( DataExceptionCommand, eSSCommand_DataException, 128 )
    UnitID roleid = 0;
    std::string tablename;
MESSAGE_END( DataExceptionCommand, roleid, tablename )

// 支付
MESSAGE_BEGIN( PayCommand, eSSCommand_Pay, 1024 )
    std::string order;      // 订单号
    std::string account;    // 账号
    std::string package;    // 套餐
    uint32_t price = 0;     // 单价
    uint32_t number = 0;    // 数量
MESSAGE_END( PayCommand, order, account, package, price, number )

// 支付回调
MESSAGE_BEGIN( PayCallbackCommand, eSSCommand_PayCallback, 256 )
    std::string order;     // 账单号
    std::string account;   // 账号
    std::string token;      // token
    TransID transid = 0;    // 事务id
MESSAGE_END( PayCallbackCommand, order, account, token, transid )

// 支付退款
MESSAGE_BEGIN( PayRefundCommand, eSSCommand_PayRefund, 256 )
    std::string order;
    std::string account;
    UnitID roleid;
    std::string package;
    uint32_t number;
MESSAGE_END( PayRefundCommand, order, account, roleid, package, number )

// 支付退款回调
MESSAGE_BEGIN( PayRefundCallbackCommand, eSSCommand_PayRefundCallback, 1024 )
    std::vector<std::string> orderlist; // 订单
    std::string token;                  // token
MESSAGE_END( PayRefundCallbackCommand, orderlist, token )

// 同步账单信息
MESSAGE_BEGIN( SyncBillingCommand, eSSCommand_SyncBilling, 256 )
    std::string order;   // 订单号
    UnitID roleid = 0;   // 角色ID
    std::string account; // 账号
    uint32_t status = 0; // 订单状态
    std::string package; // 套餐号
    uint32_t number = 0; // 购买数量
    std::string detail;  // 扩展字段
MESSAGE_END( SyncBillingCommand, order, roleid, account, status, package, number, detail )

// 同步主体进度
MESSAGE_BEGIN( SyncMainProgressCommand, eSSCommand_SyncMainProgress, 64 )
    UnitID roleid = 0;      // 角色ID
    DungeonIDList progress; // 进度
MESSAGE_END( SyncMainProgressCommand, roleid, progress )

// 创建中转区域
MESSAGE_BEGIN( CreateAreaCommand, eSSCommand_CreateArea, 128 )
    CombatID combatid = 0;
    std::string token;
    UnitIDList rolelist;
    AreaID areaid = 0;
    std::vector<int32_t> options;
MESSAGE_END( CreateAreaCommand, combatid, token, rolelist, areaid, options )

// 上报战斗结果
MESSAGE_BEGIN( ResultReportCommand, eSSCommand_ResultReport, 1024 )
    CombatID combatid = 0;
    DungeonID dungeon = 0;
    int32_t frameindex = 0;
    BattleFrames framelist;
MESSAGE_END( ResultReportCommand, combatid, dungeon, frameindex, framelist )

// 在世界频道聊天
MESSAGE_BEGIN( ChatInWorldCommand, eSSCommand_ChatInWorld, 2048 )
    bool isshutup = false;   // 是否被禁言
    std::string content;
    UnitID sender = 0;       // 发送者ID
    std::string sendername;  // 发送者
    uint32_t avatar = 0;     // 头像
    uint32_t level = 0;      // 玩家等级
    uint32_t viplevel = 0;   // 玩家VIP等级
    std::string streammedia; // 流媒体
MESSAGE_END( ChatInWorldCommand, isshutup, content, sender, sendername, avatar, level, viplevel, streammedia )

// 战斗属性同步
MESSAGE_BEGIN( SyncAttributeCommand, eSSCommand_SyncAttribute, 2048 )
    UnitID roleid = 0;       // 角色ID
    int32_t fmttype = 0;     // 阵容ID
    int32_t fmtorder = 0;    // 内部编号
    HostID dsthost = 0;      // 用于查询转发的目标主机
    HeroIDList formation;    // 阵容
    HeroIDList helpsite;     // 支援位
    std::string ultimate;    // 奥义技能
    std::string comboflow;   // 连招配置
    int64_t battlepoint = 0; // 战斗力
    BasicHeros herolist;     // 英雄属性列表
MESSAGE_END( SyncAttributeCommand, roleid, fmttype, fmtorder, dsthost, formation, helpsite, ultimate, comboflow, battlepoint, herolist )

// 查询战斗属性
MESSAGE_BEGIN( QueryAttributeCommand, eSSCommand_QueryAttribute, 128 )
    UnitID roleid = 0;       // 角色ID
    int32_t fmttype = 0;     // 阵容ID
    int32_t fmtorder = 0;    // 内部编号
    HostID dsthost = 0;      // 用于查询转发的目标主机
MESSAGE_END( QueryAttributeCommand, roleid, fmttype, fmtorder, dsthost )

// 删除战斗回放
MESSAGE_BEGIN( RemoveCombatVideoCommand, eSSCommand_RemoveCombatVideo, 1024 )
    UnitID roleid = 0;       // 角色ID
    std::vector<CombatID> combatids; // 战斗ID列表
MESSAGE_END( RemoveCombatVideoCommand, roleid, combatids )

// 竞技场挑战
MESSAGE_BEGIN( ArenaChallengeCommand, eSSCommand_ArenaChallenge, 2048 )
    UnitID roleid = 0;       // 角色ID
    UnitID opponent = 0;     // 对手ID
    int32_t fmttype = 0;     // 阵容ID
    int32_t fmtorder = 0;    // 编号
    HeroIDList formation;    // 阵容
    HeroIDList helpsite;     // 支援位
    std::string ultimate;    // 奥义技能
    std::string comboflow;   // 连招配置
    int64_t battlepoint = 0; // 战斗力
    BasicHeros herolist;     // 上阵英雄
MESSAGE_END( ArenaChallengeCommand, roleid, opponent, fmttype, fmtorder, formation, helpsite, ultimate, comboflow, battlepoint, herolist );

// 竞技场信息
MESSAGE_BEGIN( ArenaInfoCommand, eSSCommand_ArenaInfo, 256 )
    UnitID roleid = 0;
    int32_t times = 0;
MESSAGE_END( ArenaInfoCommand, roleid, times );

// 竞技场每日排名奖励
MESSAGE_BEGIN( ArenaRankRewardCommand, eSSCommand_ArenaRankReward, 1024 )
    UnitID roleid = 0;
    Resources rewards;
MESSAGE_END( ArenaRankRewardCommand, roleid, rewards );

// 竞技场单场结算
MESSAGE_BEGIN( ArenaSettleCommand, eSSCommand_ArenaSettle, 1024 )
    UnitID roleid = 0;
    int32_t season = 0;
    int64_t timestamp = 0;
    CombatID combatid = 0;
    CombatSide result = eCombatSide_None;
    int64_t change = 0;
    int64_t current = 0;
    Resources rewards;
MESSAGE_END( ArenaSettleCommand, roleid, season, timestamp, combatid, result, change, current, rewards )

// 竞技场高光奖励
MESSAGE_BEGIN( ArenaHighLightsRewardCommand, eSSCommand_ArenaHighLightsReward, 1024 )
    UnitID roleid = 0;
    Resources rewards;
MESSAGE_END( ArenaHighLightsRewardCommand, roleid, rewards );

#endif
