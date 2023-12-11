
#ifndef __SRC_BASE_TASK_H__
#define __SRC_BASE_TASK_H__

// 任务类型
enum {
    eTaskType_None = 0,          // 非法
    eTaskType_Disconnect = 1,    // 客户端连接断开
    eTaskType_SSMessage = 2,     // 命令消息
    eTaskType_Transfer = 3,      // 转发消息
    eTaskType_Traverse = 4,      // 穿透消息
    eTaskType_Response = 5,      // 回应消息
    eTaskType_HttpMessage = 6,   // 来自HTTP服务器的消息

    // gate相关
    eTaskType_Login = 7,         // 登陆(验证)
    eTaskType_LoginResult = 8,   // 登陆(验证)结果
    eTaskType_Attach = 9,        // 绑定角色
    eTaskType_AttachResult = 10, // 绑定结果
    eTaskType_RegTunnel = 11,    // 注册tunnel
    eTaskType_UnregTunnel = 12,  // 注销tunnel
    eTaskType_Kickout = 13,      // 踢出下线
    eTaskType_OfflineMsg = 14,   // 离线消息
    eTaskType_ShellCommand = 15, // GM指令

    // login相关
    eTaskType_VerifyResult = 16, // 平台验证结果

    // 数据库服务
    eTaskType_Results = 17,      // 数据库结果
    eTaskType_Datablock = 18,    // 数据块返回
    eTaskType_AutoIncrease = 19, // 自增结果
    eTaskType_Exception = 20,    // 数据异常

    // 逻辑层线程间通信
    eTaskType_RunCombat = 21,    // 运行战斗
    eTaskType_CombatResult = 22, // 战斗结果返回
    eTaskType_ReloadCombat = 23, // 重新加载战斗脚本
    eTaskType_SyncStates = 24,   // 同步状态
    eTaskType_CancelCombat = 25, // 中断战斗
};

#endif
