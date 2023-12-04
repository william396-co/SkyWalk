#pragma once

#include <set>
#include <map>
#include <tuple>
#include <vector>
#include <string>
#include <cstdint>
#include <unordered_set>

#include "support.h"

//==============================================================================
// 框架类型定义
//==============================================================================

// 运行状态
enum class RunStatus
{
    Init = 0,    // 初始化
    Running = 1, // 正在运行
    Stop = 2,    // 退出
    Pause = 3,   // 暂停服务
};

// 服务器标识
enum class ZoneMode
{
    Mix = 0,      // 混合模式, 混合模式
    Beta = 1,     // 测试模式, 测试服
    Official = 2, // 正式模式, 正式服
};

// 条件逻辑运算
enum class LogicalOp
{
    And = 1, // 与操作
    Or = 2,  // 或操作
};

// 数据修正
enum class FixType
{
    None = 0,    // 非法
    Value = 1,   // 修正数值
    Percent = 2, // 修正百分比
};

// 时间类型
enum class TimeType
{
    Invalid = 0,     // 非法类型
    Absolute = 1,    // 绝对时间
    StartZone = 2,   // 开服时间
    CreateRole = 3,  // 创角时间
    Persistence = 4, // 永久
};

// 状态
enum class SwitchState
{
    Off = 0, // 关闭
    On = 1,  // 打开
};

// 认证模式
enum class VerifyMode
{
    Block = 1, // 黑名单模式
    Allow = 2, // 白名单模式
};

// 登陆方式
enum class LoginMode
{
    None = 0,     // 非法
    Login = 1,    // 登录
    Switch = 2,   // 切换场景
    Simulate = 3, // 模拟登录
};

// AOI兴趣标记
struct InterestMask
{
    enum Value
    {
        Mark = 1,  // 被观察者
        Watch = 2, // 观察者
    };
};

// 时间周期
typedef std::pair<int64_t, int64_t> Period;

// 定义键值对
typedef std::pair<int64_t, int64_t> Range;
typedef std::pair<int32_t, int32_t> Pair;
typedef std::pair<uint32_t, uint32_t> UPair;
typedef std::pair<int64_t, int64_t> Pair64;
typedef std::pair<uint64_t, uint64_t> UPair64;

// 整型数组
typedef std::vector<int32_t> IntVec;
typedef std::vector<uint32_t> UIntVec;
typedef std::vector<int64_t> Int64Vec;
typedef std::vector<uint64_t> UInt64Vec;
typedef std::vector<Range> RangeList;
typedef std::vector<Pair> PairVec;
typedef std::vector<UPair> UPairVec;
typedef std::vector<Pair64> Pair64Vec;
typedef std::vector<UPair64> UPair64Vec;
typedef std::vector<std::string> StringList;

// 字符串
typedef std::map<std::string, std::string> StrMap;
typedef std::pair<std::string, std::string> StrPair;

// RICHTEXT占位符
#define RICHTEXT_PLACEHOLDER "##"

// 基础值
#define PERCENT_BASE_VALUE 10000
#define PERCENT( value ) ( (double)( value ) / (double)PERCENT_BASE_VALUE )

//==============================================================================
// 基础类型定义
//==============================================================================

/* 区服ID
 *
 * 高2位保留, 低14位有效
 * 也就是说最多支持16384个区
 */
typedef uint16_t ZoneID;

// 最大的区服ID
#define ZONEID_BYTES 14
#define ZONEID_MAX ( 1 << ZONEID_BYTES )

// 机房ID
#define MACHINEID_BYTES 8
#define MACHINEID_MAX ( 1 << MACHINEID_BYTES )

/*
 * 服务器编号
 *
 * 1. 单区服务器编号(8位)：
 *      高1位保留, 低7位有效
 *      也就是单个区服中最多128台服务器
 *
 * 2. 跨服集群服务器编号(8位)：
 *      跨服集群作为一个特殊的单区
 *      跨服集群服务器编号, 必须大于128, 以和单区的区分开
 *
 * 3. 全局服务器编号(32位):
 *      由16位的区服ID和8位的单区服务器编号组成
 */
typedef uint32_t HostID;             // 服务器ID
typedef std::vector<HostID> HostIDs; // 服务器ID列表

/* 生物
 * 游戏中的生命体, 包括玩家, NPC, 宠物等
 *
 * 生物ID设计:
 *      生物ID是一个无符号的48位整型数字
 *      包括 类型(4位) + 后缀(44位)
 *      1. 玩家： 区服ID(14位)+自增ID(30位)
 *      2. Npc: 服务器编号(8位)+自增ID(36位)
 *      2. 非玩家: 后缀是由 区服ID(14位)+服务器编号(8位) + 自增ID(26位)
 */

// 生物ID
typedef uint64_t UnitID;

// 生物类型(最多16种)
enum class UnitType
{
    Role = 1,    // 玩家
    Npc = 2,     // NPC, 怪物
    Pet = 3,     // 宠物
    Robot = 4,   // 机器人
    Entity = 5,  // 道具
                 // TODO: 继续添加 ...
};

// 最大的生物ID
#define UNITID_MAX ( 1ULL << 48 )

// 角色ID
#define ROLEID_BYTES 30
#define ROLEID_MAX ( 1ULL << ROLEID_BYTES )
// NPC位数
#define NPCID_BYTES 36
#define NPCID_MAX ( 1ULL << NPCID_BYTES )
// 玩家掩码
#define UNIT_ROLE_MASK 0x00000fffffffffffULL

/* 实体
 * 游戏中的物品, 包括道具, 装备, 宝石等
 *
 * 实体ID的设计:
 *      实体ID是一个无符号的64位整型数字
 *      包括 类型(4位) + 区服ID(14位) + 服务器编号(8位) + 自增ID(38位)
 */

// 实体ID
typedef uint64_t EntityID;

// ID的长度, 自增的长度
#define IDTYPE_BYTES 4
#define AUTOINCR_BYTES 38
#define AUTOINCR_MAX ( 1ULL << AUTOINCR_BYTES )

// 最大ID类型为16
enum
{
    eIDType_Entity = 1,
    eIDType_Hero = 2,
    eIDType_Mail = 3,
    eIDType_Like = 4,
    eIDType_Message = 5,
};

/* 场景
 * 包括城镇, 副本等
 *
 * 场景ID的设计:
 *      场景ID是一个无符号的32位整形数字
 *      包括 场景类型(4位) + 后缀(28位)
 *      城镇: 后缀为城镇ID
 *      副本: 后缀 = 主机ID(8位)+序号自增(20位)
 */

// 场景ID
typedef uint32_t SceneID;

// 场景类型(最多16种)
enum
{
    eSceneType_City = 1,    // 城镇
    eSceneType_Dungeon = 2, // 挂机关卡
    eSceneType_Copy = 3,    // 单人副本
    eSceneType_BrawlGround = 4, // 乱斗副本
    // TODO: 继续添加
};

// 场景操作
enum class SceneOp
{
    None = 0,  // 非法
    Sync = 1,  // 同步
    Enter = 2, // 进入
    Leave = 3, // 离开
};

#define COPYSEQ_BYTES 20 // 单人副本序号位数
#define COPYSEQ_MAX ( 1 << COPYSEQ_BYTES )
#define DUNGEONID_BYTES 16 // 挂机关卡ID位数
#define DUNGEONCHANNEL_BYTES 12 // 挂机分线位数

#define SCENE_TYPE_MASK 0xf0000000 // 场景类型掩码
#define SCENE_TYPE( id ) ( ( ( id ) >> 28 ) & 0x0F ) // 获取场景类型
#define CITY_ID( id ) ( ( id ) & ~SCENE_TYPE_MASK ) // 获取城镇ID
#define SCENE_BASEID( type ) ( SceneID( ( type ) << 28 ) ) // 场景基础ID

/*
 * 工会ID的设计
 *      工会ID是一个无符号的64位整形数字
 *      包括 占位(1位) + 区服编号(14位) + 自增ID(20位)
 * */
typedef uint64_t GuildID; // 工会ID

#define GUILDID_BYTES 20
#define GUILDID_MAX ( 1 << GUILDID_BYTES )

/*
 * 邮件ID设计:
 *      邮件ID是一个无符号的64位整型数字
 *      包括 类型(4位) + 区服ID(14位) + 服务器编号(8位) + 自增ID(38位)
 * */
typedef uint64_t MailID; // 邮件ID

// 头像和边框位数
#define AVATAR_BYTES 8
#define BORDER_BYTES 8
#define TITLE_BYTES 8

/*
 *
 *
 * */
typedef uint32_t TransID;    //
typedef uint32_t RaceType;   // 种族类型
typedef uint64_t RoleID;     // 角色ID
typedef uint64_t HeroID;     // 英雄ID
typedef int32_t SlotID;      // 格子ID
typedef int32_t ItemID;      // 道具ID
typedef int32_t QuestID;     // 任务ID
typedef int32_t MapID;       // 地图ID
typedef uint32_t CityID;     // 城镇ID
typedef uint64_t CopyID;     // 副本ID
typedef int32_t ChapterID;   // 章节ID
typedef int32_t DungeonID;   // 关卡ID
typedef uint32_t TeamID;     // 组队ID
typedef uint32_t SpellID;    // 技能ID
typedef uint32_t ArticleID;  // 商品编号
typedef int32_t SystemID;    // 系统ID
typedef int32_t TitleID;     // 称号ID
typedef uint32_t BulletinID; // 公告ID
typedef uint32_t MountID;    // 坐骑ID
typedef uint32_t ShopType;   // 商店类型
typedef uint32_t MonsterID;  // 怪物ID
typedef uint64_t MediaID;    // 媒体ID
typedef uint64_t AreaID;     // 区域ID
typedef uint64_t LogID;      // 日志ID
typedef uint64_t BossID;     // BossID
typedef uint64_t ChannelID;  // 频道ID
typedef uint64_t CombatID;   // 战斗ID
typedef uint64_t RewardID;   // 奖励ID

// 通用列表定义
typedef std::vector<ZoneID> ZoneIDList;       // 区服列表
typedef std::vector<RoleID> RoleIDList;       // 角色ID列表
typedef std::unordered_set<UnitID> UnitIDSet; // 生物ID列表
typedef std::vector<UnitID> UnitIDList;       // 生物ID列表
typedef std::vector<SceneID> SceneIDList;     // 场景ID列表
typedef std::vector<ItemID> ItemIDList;       // 物品ID列表
typedef std::vector<CityID> CityIDList;       // 城镇ID列表
typedef std::vector<MailID> MailIDList;       // 邮件ID列表
typedef std::vector<MonsterID> MonsterIDList; // 怪物ID列表
typedef std::vector<ArticleID> ArticleIDList; // 商品ID列表
typedef std::unordered_set<SystemID> SystemIDList;   // 系统ID列表
typedef std::vector<BossID> BossIDList;       // 共享BossID列表
typedef std::vector<SpellID> SpellIDList;     // 技能ID列表
typedef std::vector<DungeonID> DungeonIDList; // 关卡ID列表
typedef std::set<DungeonID> DungeonIDSet;     // 关卡ID列表
typedef std::vector<CombatID> CombatIDList;   // 战斗ID列表

enum
{
    eInvalidZoneID = 0,     // 非法区服ID
    eInvalidUnitID = 0,     // 非法生物ID
    eInvalidEntityID = 0,   // 非法物品ID
    eInvalidRoleID = 0,     // 非法角色ID
    eInvalidItemID = 0,     // 非法的道具ID
    eInvalidGuildID = 0,    // 非法的工会ID
    eInvalidQuestID = 0,    // 非法的任务ID
    eInvalidSceneID = 0,    // 非法的场景ID
    eInvalidTeamID = 0,     // 非法的组队ID
    eInvalidCopyID = 0,     // 非法的副本ID
    eInvalidHeroID = 0,     // 非法的英雄ID
    eInvalidCityID = 0,     // 非法的城镇ID
    eInvalidSkillID = 0,    // 非法技能ID
    eInvalidLevelGrade = 0, // 非法关卡评级
    eInvalidTowerFloor = 0, // 非法的楼层
};

// 种族类型
enum
{
    eRaceType_Invalid = 0, // 非法的种族类型
};

// 包裹类型
enum class BagType
{
    Invalid = 0,      // 非法的包裹类型
    EquipPanel = 1,   // 装备面板
    Package = 2,      // 背包
    Depot = 3,        // 仓库
    BuybackDepot = 4, // 回购仓库
    Max               // 包裹最大类型
};

// 邮件掩码
#define MAIL_MASK 0x0000ffffffffffffULL

// 邮件类型
enum class MailType
{
    None = 0,       // 非法类型
    User = 1,       // 用户邮件
    System = 2,     // 系统邮件
    GameMaster = 3, // GM邮件
};

// 邮件状态
enum class MailStatus
{
    None = 0,    // 非法状态
    Unread = 1,  // 未读状态
    Readed = 2,  // 已读状态
    Drawed = 3,  // 已领取状态
    Deleted = 4, // 删除状态(只有GM邮件会出现这一状态)
};

// 邮件同步方式
enum class SyncMethod
{
    None = 0, // 非法
    Sync = 1, // 同步
    Push = 2, // 推送
};

// 公告类型
enum class BulletinType
{
    None = 0,       // 非法
    User = 1,       // 玩家公告
    System = 2,     // 游戏系统公告
    GameMaster = 3, // GM公告
};

// 奖励发放形式
enum class GiftMethod
{
    None = 0,   // 非法
    Direct = 1, // 直接发放
    ByMail = 2, // 通过邮件
};

// 账单状态
enum class BillingStatus
{
    None = 0,   // 非法
    Open = 1,   // 账单打开
    Pay = 2,    // 账单支付成功
    Close = 3,  // 账单关闭
    Refund = 4, // 退款
};

// 支付相关
enum BillingType
{
    eBillingType_None = 0,        // 非法
    eBillingType_Charge = 1,      // 充值
    eBillingType_Monthcard = 2,   // 月卡
    eBillingType_BrawlPassport = 3, // 乱斗通信证
};

// 排行榜
enum RankType {
    eRankType_Min = 0,      // 非法排行
    eRankType_Arena = 1,    // 竞技场

    // 服务器内部排行榜
    eRankType_Level = 10,   // 等级排行榜
    eRankType_Max           // 最大排行ID
};
