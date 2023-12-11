
#ifndef __SRC_MESSAGE_GMPROTOCOL_H__
#define __SRC_MESSAGE_GMPROTOCOL_H__

#include "base/battle.h"
#include "base/database.h"
#include "base/resource.h"
#include "base/role.h"
#include "base/modules.h"

#include "protocol.h"

// 协议定义
enum
{
    eSSCommand_GMResult = 0x0A01,         // GM处理消息返回
    eSSCommand_QueryRole = 0x0A02,        // 查询玩家信息
    eSSCommand_ModifyRole = 0x0A03,       // 修改数据
    eSSCommand_FlushRole = 0x0A04,        // 刷新玩家数据
    eSSCommand_Shutup = 0x0A05,           // 禁言
    eSSCommand_Banned = 0x0A06,           // 封号
    eSSCommand_ActivateRole = 0x0A07,     // 角色停用/启用
    eSSCommand_SendGMMail = 0x0A08,       // 发送GM邮件
    eSSCommand_RemoveGMMail = 0x0A09,     // 移除GM邮件
    eSSCommand_ComposeGMMail = 0x0A0A,    // 撰写GM邮件
    eSSCommand_SendBulletin = 0x0A0B,     // 发送公告
    eSSCommand_RemoveBulletin = 0x0A0C,   // 移除公告
    eSSCommand_UseGiftCode = 0x0A0D,      // 使用礼包码
    eSSCommand_Login2Account = 0x0A0E,    // 角色登陆账号系统
    eSSCommand_AcquireRoleList = 0x0A0F,  // 查询角色列表
    eSSCommand_ModifyRolename = 0x0A10,   // 修改玩家名字
    eSSCommand_GetRankboard = 0x0A11,     // 获取排行榜
    eSSCommand_UpdateZoneStatus = 0x0A12, // 更新区服状态
};

// gm消息处理返回
MESSAGE_BEGIN( GMResultCommand, eSSCommand_GMResult, 1024 )
    TransID transid = 0; // 事物id
    std::string result;  // 需要返回的内容
MESSAGE_END( GMResultCommand, transid, result )

// gm查询玩家数据
MESSAGE_BEGIN( QueryRoleCommand, eSSCommand_QueryRole, 2048 )
    UnitID roleid = 0;     // 角色ID
    std::string rolename;  // 角色名
    TransID transid = 0;
    std::string version;
    std::string tablename;
    int32_t result = 0;
    Slices records;
MESSAGE_END( QueryRoleCommand, roleid, rolename, transid, version, tablename, result, records )

// 查询角色列表
MESSAGE_BEGIN( AcquireRoleListCommand, eSSCommand_AcquireRoleList, 512 )
    UnitID roleid = 0;    // 角色ID
    std::string rolename; // 角色名
    TransID transid = 0;
MESSAGE_END( AcquireRoleListCommand, roleid, rolename, transid )

// 更新数据
MESSAGE_BEGIN( ModifyRoleCommand, eSSCommand_ModifyRole, 2048 )
    UnitID roleid = 0;                // 修改角色相关数据
    Tablename tablename;              // 表名
    DBMethod method = DBMethod::None; // 增加,更新,删除
    TransID transid = 0;              // 事务id
    std::string version;              // scheme版本
    Slice value;                      // 修改的值
MESSAGE_END( ModifyRoleCommand, roleid, tablename, method, transid, version, value )

// 刷新数据
MESSAGE_BEGIN( FlushRoleCommand, eSSCommand_FlushRole, 1024 )
    bool drop = false;             // 是否丢弃缓存
    UnitID roleid = 0;             // roleid
    TransID transid = 0;           // 事务ID
    std::vector<Tablename> tables; // 表列表
MESSAGE_END( FlushRoleCommand, drop, roleid, transid, tables )

// 封号
MESSAGE_BEGIN( BannedCommand, eSSCommand_Banned, 1024 )
    TransID transid = 0;  // 事务id
    int32_t result = 0;   // 结果
    UnitID roleid = 0;    // 角色id
    uint32_t method = 0;  // 方式
    std::string reason;         // 原因
    uint64_t seconds = 0ULL;    // 封号时间(秒)
MESSAGE_END( BannedCommand, transid, result, roleid, method, reason, seconds )

// 禁言
MESSAGE_BEGIN( ShutupCommand, eSSCommand_Shutup, 128 )
    TransID transid = 0;  // 事务id
    int32_t result = 0;   // 结果
    UnitID roleid = 0;    // 角色id
    uint32_t method = 0;  // 方式
    uint64_t seconds = 0ULL;    // 封号时间
MESSAGE_END( ShutupCommand, transid, result, roleid, method, seconds )

// 启用/停用账号
MESSAGE_BEGIN( ActivateRoleCommand, eSSCommand_ActivateRole, 256 )
    TransID transid = 0; // 事务id
    std::string account; // 账号ID
    UnitID roleid = 0;   // 角色id
    uint8_t status = 0;  // 状态
MESSAGE_END( ActivateRoleCommand, transid, account, roleid, status )

// 发送GM邮件
MESSAGE_BEGIN( SendGMMailCommand, eSSCommand_SendGMMail, 2048 )
    TransID transid = 0; // 事务id
    SyncMethod method = SyncMethod::None;
    uint32_t mailid = 0;
    int64_t starttime = 0;
    int64_t expiretime = 0;
    std::string condition;
    int32_t tag = 0;
    int32_t keepdays = 0;
    std::string sendername;
    std::string title;
    std::string content;
    std::string paramlist;
    Resources attachments;
MESSAGE_END( SendGMMailCommand, transid, method, mailid, starttime, expiretime, condition, tag, keepdays, sendername, title, content, paramlist, attachments )

// 移除GM邮件
MESSAGE_BEGIN( RemoveGMMailCommand, eSSCommand_RemoveGMMail, 64 )
    TransID transid = 0; // 事务id
    uint32_t mailid = 0;
MESSAGE_END( RemoveGMMailCommand, transid, mailid )

// 撰写邮件
MESSAGE_BEGIN( ComposeGMMailCommand, eSSCommand_ComposeGMMail, 2048 )
    TransID transid = 0; // 事务id
    uint32_t mailid = 0; // 邮件ID
    UnitID roleid = 0;   // 角色ID
    int32_t tag = 0;
    int32_t keepdays = 0;
    std::string sendername;
    std::string title;
    std::string content;
    std::string paramlist;
    Resources attachments;
MESSAGE_END( ComposeGMMailCommand, transid, mailid, roleid, tag, keepdays, sendername, title, content, paramlist, attachments )

// 发送公告
MESSAGE_BEGIN( SendBulletinCommand, eSSCommand_SendBulletin, 1024 )
    TransID transid = 0;                   // 事务id
    BulletinID bulletinid = 0;             // GM公告ID
    BulletinType type = BulletinType::None;// 类型
    int32_t interval = 0;                  // 间隔
    Period sendperiod;                     // 发送周期
    int32_t style = 0;                     // 样式
    int32_t priority = 0;                  // 优先级
    int32_t showplace = 0;                 // 显示位置
    int32_t cycletimes = 0;                // 轮播次数
    std::string content;                   // 内容
    std::vector<int32_t> links;            // links
    std::vector<std::string> msgparamlist; // 消息参数
MESSAGE_END( SendBulletinCommand, transid, bulletinid, type, interval, sendperiod, style, priority, showplace, cycletimes, content, links, msgparamlist )

// 移除公告
MESSAGE_BEGIN( RemoveBulletinCommand, eSSCommand_RemoveBulletin, 64 )
    TransID transid = 0;
    BulletinID bulletinid = 0;
MESSAGE_END( RemoveBulletinCommand, transid, bulletinid )

// 使用礼包码
MESSAGE_BEGIN( UseGiftCodeCommand, eSSCommand_UseGiftCode, 1024 )
    UnitID roleid = 0;      // 角色id
    std::string code;       // 礼包码
    uint32_t channelid = 0; // 渠道id
    int32_t status = -1;    // 领取状态 0成功 >0失败原因
    Resources rewards;      // 各种奖励
MESSAGE_END( UseGiftCodeCommand, roleid, code, channelid, status, rewards )

// 角色登陆账号系统
MESSAGE_BEGIN( Login2AccountCommand, eSSCommand_Login2Account, 1024 )
    ZoneID zoneid = 0;             // 区服索引
    std::string account;           // 账号
    UnitID roleid = 0;             // 角色ID
    uint32_t avatar = 0;           // 形象
    std::string rolename;          // 角色名
    uint32_t level = 0;            // 等级
    uint32_t viplevel = 0;         // VIP等级
    time_t logintime = 0;          // 登陆时间
    time_t createtime = 0;         // 创角时间
    int64_t battlepoint = 0;       // 战斗力
    std::string storetype;         // 商店类型
    std::string storeaccount;      // 商店账号
    uint32_t onlinerolenumber = 0;  // 在线人数
    uint32_t createrolenumber = 0; // 创角个数
MESSAGE_END( Login2AccountCommand, zoneid, account, roleid, avatar, rolename, level, viplevel, logintime, createtime, battlepoint, storetype, storeaccount, onlinerolenumber, createrolenumber )

// 修改玩家名字
MESSAGE_BEGIN( ModifyRolenameCommand, eSSCommand_ModifyRolename, 128 )
    UnitID roleid = 0;
    std::string rolename;
    int32_t transid = 0;
MESSAGE_END( ModifyRolenameCommand, roleid, rolename, transid )

// 获取排行榜
MESSAGE_BEGIN( GetRankboardCommand, eSSCommand_GetRankboard, 64 )
    uint8_t type = 0;
    int32_t from = 0;
    int32_t count = 0;
    int32_t transid = 0;
MESSAGE_END( GetRankboardCommand, type, from, count, transid )

// 更新区服状态
MESSAGE_BEGIN( UpdateZoneStatusCommand, eSSCommand_UpdateZoneStatus, 64 )
    int32_t status = 0;
    int32_t transid = 0;
MESSAGE_END( UpdateZoneStatusCommand, status, transid );

#endif
