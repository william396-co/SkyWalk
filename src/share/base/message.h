#pragma once

#include <vector>
#include <cstdint>

// 通用定义
using MessageID = uint16_t;                   // 消息ID
using MessageType = uint8_t;                  // 消息类型
using MessageIDList = std::vector<MessageID>; // 消息ID列表

// 服务器通信消息类型
enum
{
    eMessageType_Invalid = 0x00,   // 非法类型
    eMessageType_Command = 0x01,   // 命令消息
    eMessageType_Transfer = 0x02,  // 转发消息
    eMessageType_Traverse = 0x03,  // 穿透消息
    eMessageType_Response = 0x04,  // 回应消息
    eMessageType_Broadcast = 0x05, // 广播消息
};

//
//
//
enum MessageDomain {
    eMessageDomain_Utils = 0x1000,    // 工具
    eMessageDomain_Role = 0x1100,     // 角色
    eMessageDomain_Battle = 0x1200,   // 战斗
    eMessageDomain_Hero = 0x1300,     // 英雄
    eMessageDomain_Item = 0x1400,     // 道具
    eMessageDomain_Dungeon = 0x1500,  // 关卡
    eMessageDomain_Mail = 0x1600,     // 邮件
    eMessageDomain_Quest = 0x1600,    // 任务
    eMessageDomain_Social = 0x1800,   // 社会化
    eMessageDomain_Guild = 0x1900,    // 公会
    eMessageDomain_Zone = 0x1A00,     // 个人空间
    eMessageDomain_Activity = 0x1B00, // 活动
    eMessageDomain_Arena = 0x1C00,    // 竞技场
    eMessageDomain_Mask = 0xFF00,     // 通用
};

//
// 通信包头定义
//

#pragma pack( 1 )

// 服务器之间通信的包头
struct SSHead
{
    MessageType type = 0; // 类型
    uint8_t flags = 0;    // 标志位
    MessageID cmd = 0;    // 命令字
    uint64_t size = 0;    // 长度
    SSHead() {}
    SSHead( MessageType t, MessageID c ) : type(t), cmd(c) {}
    SSHead( MessageID c ) : type(eMessageType_Command), cmd(c) {}
};

#pragma pack()
