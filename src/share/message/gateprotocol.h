
#ifndef __SRC_MESSAGE_GATEPROTOCOL_H__
#define __SRC_MESSAGE_GATEPROTOCOL_H__

#include "protocol.h"

enum
{
    eSSCommand_Kickout = 0x0101,          // 踢出玩家
    eSSCommand_KickoutAck = 0x0102,       // 确认玩家被踢出
    eSSCommand_Logout = 0x0103,           // 玩家登出
    eSSCommand_RegTunnel = 0x0104,        // 注册通道
    eSSCommand_UnregTunnel = 0x0105,      // 注销通道
    eSSCommand_JoinChannel = 0x0106,      // 加入频道
    eSSCommand_QuitChannel = 0x0107,      // 退出频道
    eSSCommand_Credential = 0x0108,       // 验证凭据
    eSSCommand_SyncOnlineStatus = 0x0109, // 同步在线状态
    eSSCommand_RoleList = 0x010A,         // 角色列表通知
    eSSCommand_ReportLatency = 0x010B,    // 通报延迟
    eSSCommand_SyncOnlineCount = 0x010C,  // 同步在线人数
};

// 踢出玩家
// roleid == 0 && account.empty()
// 踢出服务器中所有玩家
MESSAGE_BEGIN( KickoutCommand, eSSCommand_Kickout, 128 )
    UnitID roleid = 0;      // 角色ID
    std::string account;    // 账号ID
    int32_t reason = 0;     // 原因(可选的)
    uint32_t transid = 0;   // 事务ID(可选的)
MESSAGE_END( KickoutCommand, roleid, account, reason, transid )

// 确认玩家被踢出
MESSAGE_BEGIN( KickoutAckCommand, eSSCommand_KickoutAck, 128 )
    UnitID roleid = 0;      // 角色ID
    std::string account;    // 账号ID
    uint32_t transid = 0;   // 事务ID(可选的)
MESSAGE_END( KickoutAckCommand, roleid, account, transid )

// 玩家登出
MESSAGE_BEGIN( LogoutCommand, eSSCommand_Logout, 128 )
    UnitID roleid = 0;     // 角色ID
    int32_t reason = 0;    // 0-正常登出；!=0-被踢出的原因
    sid_t clientsid = 0;   // 客户端的会话ID
    int64_t timestamp = 0; // 登出时间
MESSAGE_END( LogoutCommand, roleid, reason, clientsid, timestamp )

// 注册通道
MESSAGE_BEGIN( RegTunnelCommand, eSSCommand_RegTunnel, 64 )
    UnitID roleid = 0;                  // 角色ID
    HostID hostid = 0;                  // 服务器ID
    HostType hosttype = HostType::None; // 服务器类型
MESSAGE_END( RegTunnelCommand, roleid, hostid, hosttype )

// 注销通道
MESSAGE_BEGIN( UnregTunnelCommand, eSSCommand_UnregTunnel, 64 )
    UnitID roleid = 0;                  // 角色ID
    HostID hostid = 0;                  // 服务器ID
    HostType hosttype = HostType::None; // 服务器类型
MESSAGE_END( UnregTunnelCommand, roleid, hostid, hosttype )

// 加入频道消息
MESSAGE_BEGIN( JoinChannelCommand, eSSCommand_JoinChannel, 64 )
    UnitID roleid = 0;       // 角色ID
    uint8_t channel = 0;     // 频道
    ChannelID channelid = 0; // 频道ID
MESSAGE_END( JoinChannelCommand, roleid, channel, channelid )

// 退出频道
MESSAGE_BEGIN( QuitChannelCommand, eSSCommand_QuitChannel, 64 )
    UnitID roleid = 0;       // 角色ID
    uint8_t channel = 0;     // 频道
    ChannelID channelid = 0; // 频道ID
MESSAGE_END( QuitChannelCommand, roleid, channel, channelid )

// 验证凭据
MESSAGE_BEGIN( CredentialCommand, eSSCommand_Credential, 1024 )
    std::string account;
    std::string token;
    uint32_t transid = 0;
    std::string extrainfo; // 平台返回的额外信息, storeType, storeAccount, rninfo
MESSAGE_END( CredentialCommand, account, token, transid, extrainfo )

// 同步在线状态
MESSAGE_BEGIN( SyncOnlineStatusCommand, eSSCommand_SyncOnlineStatus, 65536 )
    HostID hostid = 0;
    std::vector<std::pair<UnitID, HostIDs>> onlinelist;
MESSAGE_END( SyncOnlineStatusCommand, hostid, onlinelist )

// 设置角色列表
MESSAGE_BEGIN( SyncRoleListCommand, eSSCommand_RoleList, 256 )
    std::string account;
    UnitIDList rolelist;
MESSAGE_END( SyncRoleListCommand, account, rolelist )

// 通报延迟时间
MESSAGE_BEGIN( ReportLatencyCommand, eSSCommand_ReportLatency, 64 )
    UnitID roleid = 0;
    int64_t latency = 0;
MESSAGE_END( ReportLatencyCommand, roleid, latency )

// 同步状态
MESSAGE_BEGIN( SyncOnlineCountCommand, eSSCommand_SyncOnlineCount, 128 )
    enum Method
    {
        NONE = 0,   // 非法
        LOGIN = 1,  // 登录
        LOGOUT = 2, // 登出
    };
    uint8_t method = Method::NONE;
    UnitID roleid = 0;
    std::string account;
    uint32_t onlinecount = 0;
MESSAGE_END( SyncOnlineCountCommand, method, roleid, account, onlinecount )

#endif
