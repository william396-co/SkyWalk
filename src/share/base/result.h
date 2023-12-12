
#ifndef __SRC_BASE_RESULT_H__
#define __SRC_BASE_RESULT_H__

#include <stdint.h>

enum
{
    eHttpResult_Success = 0,                     // 成功
    eHttpResult_InvalidRequest = 100001,         // 非法请求
    eHttpResult_Timeout = 100002,                // 请求超时
    eHttpResult_Restricted = 100003,             // 访问受限
    eHttpResult_PermissionDenied = 100004,       // 权限不足
    eHttpResult_InternalError = 100005,          // 内部错误
    eHttpResult_InvalidAffid = 100010,           // 非法的渠道信息
    eHttpResult_InvalidZoneid = 100011,          // 非法的区服信息
    eHttpResult_ZoneIsShutdown = 100012,         // 区服已经关闭
    eHttpResult_ZoneIsMaintain = 100013,         // 区服正在维护
    eHttpResult_ZoneIsBusy = 100014,             // 区服繁忙
    eHttpResult_AccountIsBlock = 100015,         // 黑名单账号
    eHttpResult_AccountInDebugMode = 100016,     // 账号处于调试模式
    eHttpResult_VerifyFailed = 100017,           // 平台验证失败
    eHttpResult_InvalidAccount = 100018,         // 非法的账号
    eHttpResult_InvalidRoleID = 100020,          // 非法的角色
    eHttpResult_HotfixInProgress = 100021,       // 热更进行中
    eHttpResult_OutofMemory = 100022,            // 内存溢出
    eHttpResult_InvalidReloadParams = 100023,    // 热更参数出错
    eHttpResult_MailTimeIsInvalid = 100024,      // 邮件时间出错
    eHttpResult_MailReceiverInvalid = 100025,    // 接收者非法
    eHttpResult_ComposeTooFast = 100026,         // 邮件发送的太快了
    eHttpResult_MailIDConflict = 100027,         // 邮件ID冲突
    eHttpResult_MailContentLong = 100028,        // 邮件内容太多了
    eHttpResult_MailAttachmentsTooMuch = 100029, // 邮件附件太多了
    eHttpResult_MailIDInvalid = 100030,          // 邮件ID非法
    eHttpResult_MailInvalidItem = 100031,        // 邮件附件道具非法
    eHttpResult_LoadTimeout = 100032,            // 加载超时
    eHttpResult_SchemeVersionError = 100033,     // 数据表结构版本不一致
    eHttpResult_TablenameError = 100034,         // 数据表名错误
    eHttpResult_ModifyValueIsNull = 100035,      // 修改数据为空
    eHttpResult_RoleIsOnline = 100036,           // 玩家在线无法修改数据
    eHttpResult_AcquireRoleListInvalid = 100037, // 查询角色列表参数非法
    eHttpResult_CantModifyKeyField = 100038,     // 无法修改索引和主键字段
    eHttpResult_RankTypeInvalid = 100039,        // 排行榜类型非法
    eHttpResult_ReasonTooLong = 100040,          // 封禁原因太长了
    eHttpResult_RolenameTooLong = 100041,        // 名字太长了
    eHttpResult_RolenameInvalid = 100042,        // 名字非法
    eHttpResult_DuplicateName = 100043,          // 重复的名字
    eHttpResult_InvalidFlushParams = 100044,     // 刷新角色参数出错
    eHttpResult_PayCallbackZoneError = 100045,   // 错误的区服
    eHttpResult_PayCallbackOrderError = 100046,  // 账单错误
    eHttpResult_PayCallbackConfigError = 100047, // 配置错误
    eHttpResult_PayRefundZoneError = 100048,     // 错误的区服
    eHttpResult_PayRefundOrderlistEmpty = 100049,// 账单列表为空
};

#endif
