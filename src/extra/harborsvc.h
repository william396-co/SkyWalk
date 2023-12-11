
#ifndef __SRC_EXTRA_HARBORSERVICE_H__
#define __SRC_EXTRA_HARBORSERVICE_H__

#include <cassert>

#include "base/types.h"
#include "io/service.h"
#include "utils/thread.h"
#include "protocol/types.h"

class MessageFactory;

class CHarborService : public HarborService
{
public:
    CHarborService(
        const char * module,
        ISysConfigfile * sysconf,
        utils::IWorkThread * thread );

    virtual ~CHarborService();
    virtual void * initIOContext();
    virtual void finalIOContext( void * context );
    virtual bool post( void * iocontext, int32_t type, void * task );

public:
    // 转发消息
    int32_t transfer( sid_t sid,
        UnitID roleid, MessageID cmd, GameMessage * msg, const std::string & oob = "" );
    int32_t transferByHostID( HostID id,
        UnitID roleid, MessageID cmd, GameMessage * msg, const std::string & oob = "" );

    // 中转消息
    int32_t transfer( sid_t sid,
        UnitID roleid, MessageID cmd, const Slice & msg, const std::string & oob = "" );
    int32_t transferByHostID( HostID id,
        UnitID roleid, MessageID cmd, const Slice & msg, const std::string & oob = "" );

    // 穿透消息
    int32_t traverse( sid_t sid,
        sid_t clientsid, MessageID cmd, GameMessage * msg, const std::string & oob = "" );
    int32_t traverseByHostID( HostID id,
        sid_t clientsid, MessageID cmd, GameMessage * msg, const std::string & oob = "" );

    // 回应消息
    int32_t reply( sid_t sid,
        UnitID roleid, MessageID cmd, GameMessage * msg );
    int32_t replyByHostID( HostID id,
        UnitID roleid, MessageID cmd, GameMessage * msg );

    // 广播消息到网关
    // type     - 频道类型
    // channel  - 频道
    int32_t broadcast2Channel( const UnitIDList & rolelist, MessageID cmd, GameMessage * msg );
    int32_t broadcast2Channel(
        uint8_t type, ChannelID channel, MessageID cmd, GameMessage * msg, UnitID exceptid = 0 );

private:
    void traverseErrorcode( sid_t sid, sid_t id, MessageID cmd, int32_t result );
    void transferErrorcode( sid_t sid, UnitID id, MessageID cmd, int32_t result );
    GameMessage * parse( sid_t sid, UnitID roleid, MessageFactory * factory, MessageID cmd, Slice msgbody );
};

#endif
