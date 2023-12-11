
#include <google/protobuf/message.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/descriptor_database.h>

#include "type.pb.h"
#include "messagefactory.h"

void MessageFactory::initialize()
{
    regAutomatic();
}

void MessageFactory::finalize()
{
    for ( const auto & val : m_MessageTable ) {
        GameMessage * prototype = val.second.prototype;
        if ( prototype ) {
            delete prototype;
        }
    }

    m_MessageTable.clear();
}

bool MessageFactory::add( MessageID id, GameMessage * prototype, HostType target, MessageID responseid )
{
    MessageDefine define;
    define.target = target;
    define.prototype = prototype;
    define.responseid = responseid;

    auto result = m_MessageTable.find( id );
    assert( result == m_MessageTable.end() && "MessageID conflict" );
    if ( result != m_MessageTable.end() ) {
        return false;
    }

    m_MessageTable[id] = define;

    return true;
}

bool MessageFactory::del( MessageID id )
{
    auto result = m_MessageTable.find( id );
    if ( result == m_MessageTable.end() ) {
        return false;
    }

    GameMessage * message = result->second.prototype;
    delete message;
    m_MessageTable.erase( result );

    return true;
}

HostType MessageFactory::target( MessageID id ) const
{
    auto result = m_MessageTable.find( id );
    if ( result == m_MessageTable.end() ) {
        return HostType::None;
    }

    return result->second.target;
}

MessageID MessageFactory::responseid( MessageID id ) const
{
    auto result = m_MessageTable.find( id );
    if ( result == m_MessageTable.end() ) {
        return 0;
    }

    return result->second.responseid;
}

GameMessage * MessageFactory::prototype( MessageID id ) const
{
    auto result = m_MessageTable.find( id );
    if ( result == m_MessageTable.end() ) {
        return nullptr;
    }

    return result->second.prototype->New();
}

bool MessageFactory::get( MessageID id, MessageDefine * message ) const
{
    auto result = m_MessageTable.find( id );
    if ( result != m_MessageTable.end() ) {
        message->target = result->second.target;
        message->responseid = result->second.responseid;
        message->prototype = result->second.prototype->New();
    }

    return result != m_MessageTable.end();
}

void MessageFactory::regAutomatic()
{
    auto pool = google::protobuf::DescriptorPool::generated_pool();
    auto factory = google::protobuf::MessageFactory::generated_factory();
    assert( pool != nullptr && factory != nullptr );

    std::vector<std::string> filenames;
    auto db = pool->internal_generated_database();
    assert( db != nullptr && "generate message db failed" );
    db->FindAllFileNames( &filenames );
    for ( const auto & filename : filenames ) {
        auto filedesc = pool->FindFileByName( filename );
        if ( filedesc != nullptr ) {
            // 消息
            int msgcount = filedesc->message_type_count();
            for ( int i = 0; i < msgcount; ++i ) {
                // 描述符
                auto msgdesc = filedesc->message_type( i );
                if ( msgdesc == nullptr ) {
                    continue;
                }
                // 未注册消息ID 或者 没有目标的消息
                auto msgid = msgdesc->options().GetExtension( msg::msgid );
                auto target = (HostType)msgdesc->options().GetExtension( msg::target );
                if ( msgid == 0 || target == HostType::None ) {
                    continue;
                }
                // 原型
                auto prototype = factory->GetPrototype( msgdesc );
                if ( prototype == nullptr ) {
                    continue;
                }
                add( msgid, prototype->New(), target, msgdesc->options().GetExtension( msg::ackid ) );
            }
        }
    }
}

///////////////////////////////////////////////////////////////////////////////

void MessageFactory::regManual()
{
    //
    // enum
    // {
    //     HostType::None          = 0,        // 非法的主机节点
    //     HostType::Gate          = 1,        // 网关服务器
    //     HostType::Scene         = 2,        // 场景服务器
    //     HostType::World         = 3,        // 世界服务器
    //     HostType::Copy          = 4,        // 副本服务器
    //     HostType::Social        = 5,        // 社交服务器
    //     HostType::Data          = 6,        // 数据服务器
    //     HostType::Login         = 7,        // 登陆服务器
    //     HostType::Master        = 8,        // 管理服务器
    //     HostType::Global        = 11,       // 集群管理服务器
    //     HostType::Transit       = 12,       // 中转服务器
    //     HostType::Cross         = 13,       // 跨服服务器
    //     HostType::Any           = 255,      // 任何节点
    // };
    // TODO: 注册所有消息
}
