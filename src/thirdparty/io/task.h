
#ifndef __SRC_IO_TASK_H__
#define __SRC_IO_TASK_H__

#include "io.h"

#include "base/types.h"
#include "base/task.h"
#include "base/message.h"
#include "base/endpoint.h"
#include "utils/slice.h"
#include "protocol/types.h"

class HttpRequest;

// 客户端连接中断
struct DisconnectTask
{
    sid_t sid = 0;
    std::string account;
    UnitID roleid = 0;

    DisconnectTask( sid_t id )
        : sid( id ), roleid( 0 ) {}
    DisconnectTask( sid_t id, const std::string & acc, UnitID rid )
        : sid( id ), account( acc ), roleid( rid ) {}
};

// 转发消息
struct TransferTask
{
    sid_t sid = 0;
    MessageID msgid = 0;
    UnitID roleid = 0;
    std::string oob;
    GameMessage * message = nullptr;

    TransferTask() = default;
    TransferTask( sid_t id_, MessageID msgid_, UnitID roleid_, const std::string & oob_, GameMessage * message_ )
        : sid( id_ ), msgid( msgid_ ), roleid( roleid_ ), oob( oob_ ), message( message_ ) {}
};

// 穿透消息
struct TraverseTask
{
    sid_t sid = 0;
    MessageID msgid = 0;
    sid_t fromsid = 0;
    std::string oob;
    GameMessage * message = nullptr;

    TraverseTask() = default;
    TraverseTask( sid_t id_, MessageID msgid_, GameMessage * message_ )
        : sid( id_ ), msgid( msgid_ ), fromsid( id_ ), message( message_ ) {}
    TraverseTask( sid_t id_, MessageID msgid_, sid_t fromsid_, const std::string & oob_, GameMessage * message_ )
        : sid( id_ ), msgid( msgid_ ), fromsid( fromsid_ ), oob( oob_ ), message( message_ ) {}
};

// 回应消息
struct ResponseTask
{
    MessageID msgid = 0;
    Slice message;

    ResponseTask() = default;
    ResponseTask( MessageID id, const Slice & msg ) : msgid( id ), message( msg ) {}

    static void recycle( int32_t type, void * task, int32_t interval ) {
        delete static_cast<ResponseTask *>( task );
    }
};

// Http请求
struct HttpRequestTask
{
    sid_t sid = 0;
    HttpRequest * request = nullptr;

    HttpRequestTask() = default;
    HttpRequestTask( sid_t id, HttpRequest * req ) : sid( id ), request( req ) {}
};

#endif
