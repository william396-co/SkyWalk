
#include <google/protobuf/message.h>

#include "base/base.h"
#include "utils/types.h"
#include "utils.pb.h"
#include "io/task.h"
#include "message/protocol.h"
#include "protocol/messagefactory.h"

#include "harborsvc.h"

CHarborService::CHarborService( const char * module, ISysConfigfile * sysconf, utils::IWorkThread * thread )
    : HarborService( module, sysconf, thread )
{}

CHarborService::~CHarborService()
{}

void * CHarborService::initIOContext()
{
    MessageFactory * factory = new MessageFactory;
    if ( factory != nullptr ) {
        factory->initialize();
        LOG_DEBUG( "HarborService load {} Message(s) from google::protobuf::DescriptorPool .\n", factory->msgcount() );
    }
    return factory;
}

void CHarborService::finalIOContext( void * context )
{
    MessageFactory * factory = static_cast<MessageFactory *>( context );
    if ( factory != nullptr ) {
        factory->finalize();
        delete factory;
    }
}

bool CHarborService::post( void * iocontext, int32_t type, void * task )
{
    void * newtask = task;
    MessageFactory * factory = static_cast<MessageFactory *>( iocontext );

    if ( type == eTaskType_SSMessage ) {
        SSMessage * message = static_cast<SSMessage *>( task );
        HostType targetype = factory->target( message->head.cmd );

        // 转发给自己的消息才会处理
        if ( m_HostType == targetype || targetype == HostType::Any ) {
            switch ( message->head.type ) {
                case eMessageType_Transfer: {
                    // 转发消息
                    UnitID roleid = static_cast<TransferMessage *>( message )->roleid;
                    GameMessage * msg = parse( 0, roleid,
                        factory, message->head.cmd, static_cast<TransferMessage *>( message )->msgbody );
                    if ( msg == nullptr ) {
                        transferErrorcode( message->sid, roleid, message->head.cmd, msg::utils::ParseFailed );
                        return false;
                    }

                    // 提交到逻辑层
                    type = eTaskType_Transfer;
                    newtask = (void *)new TransferTask( message->sid,
                        message->head.cmd, roleid,
                        static_cast<TransferMessage *>( message )->oob.ToString(),
                        msg );
                    delete message;
                } break;

                case eMessageType_Traverse: {
                    sid_t sid = static_cast<TraverseMessage *>( message )->fromsid;
                    GameMessage * msg = parse(
                        sid, 0, factory, message->head.cmd, static_cast<TraverseMessage *>( message )->msgbody );
                    if ( msg == nullptr ) {
                        traverseErrorcode( message->sid, sid, message->head.cmd, msg::utils::ParseFailed );
                        return false;
                    }

                    // 提交到逻辑层
                    type = eTaskType_Traverse;
                    newtask = (void *)new TraverseTask( message->sid,
                        message->head.cmd, sid,
                        static_cast<TransferMessage *>( message )->oob.ToString(),
                        msg );
                    delete message;
                } break;
            }
        }
    }

    return m_WorkThread->post( type, newtask );
}

int32_t CHarborService::transfer( sid_t sid, UnitID roleid, MessageID cmd, GameMessage * msg, const std::string & oob )
{
    // 序列化到body中
    std::string body;
    msg->SerializeToString( &body );
    return transfer( sid, roleid, cmd, Slice( body.data(), body.size() ), oob );
}

int32_t CHarborService::transferByHostID( HostID id, UnitID roleid, MessageID cmd, GameMessage * msg, const std::string & oob )
{
    return transfer( m_Harbor.sid( id ), roleid, cmd, msg, oob );
}

int32_t CHarborService::transfer( sid_t sid, UnitID roleid, MessageID cmd, const Slice & msg, const std::string & oob )
{
    TransferMessage packet;
    // 赋值
    packet.roleid = roleid;
    packet.head.cmd = cmd;
    packet.head.type = eMessageType_Transfer;
    packet.msgbody = msg;
    packet.oob = oob;

    return HarborService::send( sid, &packet );
}

int32_t CHarborService::transferByHostID( HostID id, UnitID roleid, MessageID cmd, const Slice & msg, const std::string & oob )
{
    return transfer( m_Harbor.sid( id ), roleid, cmd, msg, oob );
}

int32_t CHarborService::traverse( sid_t sid, sid_t clientsid, MessageID cmd, GameMessage * msg, const std::string & oob )
{
    // 序列化到body中
    std::string body;
    msg->SerializeToString( &body );

    TraverseMessage packet;
    // 赋值
    packet.fromsid = clientsid;
    packet.msgbody = Slice( body.data(), body.size() );
    packet.oob = oob;
    packet.head.cmd = cmd;
    packet.head.type = eMessageType_Traverse;

    return HarborService::send( sid, &packet );
}

int32_t CHarborService::traverseByHostID( HostID id, sid_t clientsid, MessageID cmd, GameMessage * msg, const std::string & oob )
{
    return traverse( m_Harbor.sid( id ), clientsid, cmd, msg, oob );
}

int32_t CHarborService::reply( sid_t sid, UnitID roleid, MessageID cmd, GameMessage * msg )
{
    // 序列化到body中
    std::string body;
    msg->SerializeToString( &body );

    ResponseMessage packet;
    // 赋值
    packet.roleid = roleid;
    packet.msgbody = Slice( body.data(), body.size() );
    packet.head.cmd = cmd;
    packet.head.type = eMessageType_Response;

    return HarborService::send( sid, &packet );
}

int32_t CHarborService::replyByHostID( HostID id, UnitID roleid, MessageID cmd, GameMessage * msg )
{
    return reply( m_Harbor.sid( id ), roleid, cmd, msg );
}

int32_t CHarborService::broadcast2Channel( uint8_t type, ChannelID channel, MessageID cmd, GameMessage * msg, UnitID exceptid )
{
    std::string body;
    msg->SerializeToString( &body );

    BroadcastMessage packet;
    packet.channel = type;
    packet.channelid = channel;
    packet.exceptid = exceptid;
    packet.msgbody = Slice( body.data(), body.size() );
    packet.head.cmd = cmd;
    packet.head.type = eMessageType_Broadcast;
    return HarborService::broadcast( HostType::Gate, &packet );
}

int32_t CHarborService::broadcast2Channel( const UnitIDList & rolelist, MessageID cmd, GameMessage * msg )
{
    std::string body;
    msg->SerializeToString( &body );

    BroadcastMessage packet;
    packet.channel = eChannel_RoleList;
    packet.channelid = 0;
    packet.rolelist = rolelist;
    packet.msgbody = Slice( body.data(), body.size() );
    packet.head.cmd = cmd;
    packet.head.type = eMessageType_Broadcast;
    return HarborService::broadcast( HostType::Gate, &packet );
}

void CHarborService::traverseErrorcode( sid_t sid, sid_t id, MessageID cmd, int32_t result )
{
    msg::utils::ErrorcodeNotify msg;
    msg.set_cmd( cmd );
    msg.set_result( result );
    traverse( sid, id, msg::Errorcode_S, &msg );
}

void CHarborService::transferErrorcode( sid_t sid, UnitID id, MessageID cmd, int32_t result )
{
    msg::utils::ErrorcodeNotify msg;
    msg.set_cmd( cmd );
    msg.set_result( result );
    transfer( sid, id, msg::Errorcode_S, &msg );
}

GameMessage * CHarborService::parse( sid_t sid, UnitID roleid, MessageFactory * factory, MessageID cmd, Slice msgbody )
{
    GameMessage * message = factory->prototype( cmd );
    if ( unlikely( message == nullptr ) ) {
        LOG_ERROR( "CHarborService::parse(Sid:{}, RoleID:{}) this MessageID({:#06x}) is not Registed in the MessageFactory .\n",
            sid, roleid, cmd );
        return nullptr;
    }

    if ( unlikely( !message->ParseFromArray( msgbody.data(), msgbody.size() ) ) ) {
        LOG_ERROR( "CHarborService::parse(Sid:{}, RoleID:{}) this MessageID({:#06x}) parse failed .\n", sid, roleid, cmd );
        delete message;
        return nullptr;
    }

    return message;
}
