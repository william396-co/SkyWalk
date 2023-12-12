
#ifndef __SRC_MASTERSERVER_CMDHANDLER_H__
#define __SRC_MASTERSERVER_CMDHANDLER_H__

#include "io/io.h"
#include "base/types.h"
#include "base/message.h"

struct SSMessage;
class HttpRequest;

namespace master {

class CommandHandler
{
public:
    CommandHandler() {}
    ~CommandHandler() {}
    static void process( SSMessage * cmd );
    static void process( sid_t sid, HttpRequest * request );

private:
    // 处理游戏礼包码消息
    static void useGiftCode( SSMessage * cmd );
    // 处理角色登陆
    static void login2Account( SSMessage * cmd );
    // 服务器返回状态
    static void sendStatus( SSMessage * cmd );
    // gm消息回应
    static void gmResult( SSMessage * cmd );
    // 接受玩家数据
    static void queryRole( SSMessage * cmd );
    // 接受修改数据
    static void modifyRole( SSMessage * cmd );
    // 启动/停用账号
    static void activateRole( SSMessage * cmd );
    // 对时
    static void ntpdate( SSMessage * cmd );

private:
    // 查询服务器状态
    static int32_t status( sid_t sid, HttpRequest * request );
    // 热更
    static int32_t reload( sid_t sid, HttpRequest * request );
    // 更新状态
    static int32_t update( sid_t sid, HttpRequest * request );
    // 执行GM指令
    static int32_t doShellCmd( sid_t sid, HttpRequest * request );

    // 查询角色
    static int32_t queryRole( sid_t sid, HttpRequest * request );
    // 查询角色列表
    static int32_t acquireRole( sid_t sid, HttpRequest * request );
    // 修改角色数据
    static int32_t modifyRole( sid_t sid, HttpRequest * request );
    // 刷新玩家数据
    static int32_t flushRole( sid_t sid, HttpRequest * request );
    // 封号
    static int32_t banned( sid_t sid, HttpRequest * request );
    // 禁言
    static int32_t shutup( sid_t sid, HttpRequest * request );
    // 改名
    static int32_t rename( sid_t sid, HttpRequest * request );
    // 启用/停用
    static int32_t activate( sid_t sid, HttpRequest * request );

    // 添加GM邮件
    static int32_t addMail( sid_t sid, HttpRequest * request );
    // 移除邮件
    static int32_t removeMail( sid_t sid, HttpRequest * request );
    // 撰写邮件
    static int32_t composeMail( sid_t sid, HttpRequest * request );

    // 获取排行榜
    static int32_t getRankboard( sid_t sid, HttpRequest * request );
    // 更新排行榜
    static int32_t updateRank( sid_t sid, HttpRequest * request );

    // 发送公告
    static int32_t sendBulletin( sid_t sid, HttpRequest * request );
    // 移除公告
    static int32_t removeBulletin( sid_t sid, HttpRequest * request );

    // 支付回调
    static int32_t payCallback( sid_t sid, HttpRequest * request );
    // 支付退款回调
    static int32_t payRefundCallback( sid_t sid, HttpRequest * request );
};

} // namespace master

#endif
