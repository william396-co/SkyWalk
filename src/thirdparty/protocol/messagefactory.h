#pragma once

#include <unordered_map>

#include "types.h"
#include "utils/types.h"
#include "base/endpoint.h"

struct MessageDefine
{
    HostType target;         // 路由目的地
    MessageID responseid;    // 回应消息ID
    GameMessage * prototype; // 消息原型

    MessageDefine()
        : target( HostType::None ),
          responseid( 0 ),
          prototype( nullptr )
    {}
};

//
// 消息工厂
//
class MessageFactory
{
public:
    MessageFactory() {}
    ~MessageFactory() {}

    // 初始化所有消息
    void initialize();
    void finalize();

public:
    // 注册消息
    bool add( MessageID id,
        GameMessage * prototype,
        HostType target = HostType::None, MessageID responseid = 0 );

    // 反注册消息
    bool del( MessageID id );

    // 获取目标
    HostType target( MessageID id ) const;

    // 获取回应ID
    MessageID responseid( MessageID id ) const;

    // 获取消息原型
    GameMessage * prototype( MessageID id ) const;

    // 获取消息
    bool get( MessageID id, MessageDefine * message ) const;

    // 消息个数
    size_t msgcount() const { return m_MessageTable.size(); }

private :
    void regManual();
    void regAutomatic();

private:
    std::unordered_map<MessageID, MessageDefine> m_MessageTable;
};
