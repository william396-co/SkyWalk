
#include <algorithm>
#include <stdlib.h>

#include "base/base.h"
#include "utils/types.h"
#include "utils/xtime.h"
#include "utils/utility.h"
#include "utils/streambuf.h"

#include "message/protocol.h"

#include "task.h"
#include "service.h"
#include "framework.h"

// Harbor会话
class HarborSession : public IIOSession
{
public:
    HarborSession( Harbor::NodeType type, HarborService * s, int32_t seconds )
        : m_Type( type ),
          m_Service( s ),
          m_TimeoutSeconds( seconds )
    {}

    virtual ~HarborSession()
    {}

    virtual int32_t onStart()
    {
        // 设置超时时间
        setTimeout( m_TimeoutSeconds );

        // 连接远程服务器的情况下
        if ( m_Type == Harbor::CONNECT ) {
            // 设置保活时间
            setKeepalive( m_TimeoutSeconds / 5 );

            // 提交到逻辑层
            m_Service->sendRegisterCommand( id() );
        }

        return 0;
    }

    virtual ssize_t onProcess( const char * buffer, size_t nbytes )
    {
        ssize_t nprocess = 0;

        for ( ;; ) {
            SSHead head;
            size_t nleft = nbytes - nprocess;
            const char * buf = buffer + nprocess;

            if ( nleft < sizeof( SSHead ) ) {
                break;
            }

            StreamBuf unpack( buf, nleft );
            unpack.decode( head.type );
            unpack.decode( head.flags );
            unpack.decode( head.cmd );
            unpack.decode( head.size );

            // 合法的数据包
            if ( nleft < head.size + sizeof( SSHead ) ) {
                break;
            }

            SSMessage * msg = GeneralDecoder::instance().decode(
                id(), head, Slice( buf + sizeof( SSHead ), head.size ) );
            if ( msg != nullptr ) {
                onMessage( msg );
            }

            nprocess += ( head.size + sizeof( SSHead ) );
        }

        return nprocess;
    }

    virtual int32_t onTimeout()
    {
        LOG_WARN( "HarborSession::onTimeout(Sid:{}, {}) .\n", id(), endpoint() );
        return m_Type == Harbor::CONNECT ? 0 : -1;
    }

    virtual int32_t onKeepalive()
    {
        if ( m_Type == Harbor::CONNECT ) {
            PingCommand cmd;
            send( &cmd );
        }

        return 0;
    }

    virtual int32_t onError( int32_t result )
    {
        LOG_ERROR( "HarborSession::onError(Sid:{}, {}): {:#010x} .\n", id(), endpoint(), result );
        return m_Type == Harbor::CONNECT ? 0 : -1;
    }

    virtual void onShutdown( int32_t way )
    {
        LOG_INFO( "HarborSession::onShutdown(Sid:{}, {}): way={} .\n", id(), endpoint(), way );

        if ( way == 1 ) {
            m_Service->POST<SSMessage>( iocontext(), eTaskType_SSMessage, new UnregisterCommand( id(), endpoint() ) );
        }
    }

private:
    // 发送
    int32_t send( SSMessage * msg )
    {
        Slice buf = msg->encode();
        if ( unlikely( buf.empty() ) ) {
            LOG_FATAL( "HarborSession::send(Sid:{}) : this Message(CMD:{:#06x}) encode() failed .\n", id(), msg->head.cmd );
            return -1;
        }

        return IIOSession::send(
            buf.data(), buf.size(), true );
    }

    // 完整的逻辑包
    void onMessage( SSMessage * message )
    {
        switch ( message->head.type ) {
            case eMessageType_Transfer:
            case eMessageType_Traverse:
                m_Service->POST<SSMessage>( iocontext(), eTaskType_SSMessage, message );
                break;

            case eMessageType_Command: {
                switch ( message->head.cmd ) {
                    case eSSCommand_Ping: {
                        if ( m_Type == Harbor::ACCEPT ) {
                            PingCommand cmd;
                            send( &cmd );
                        }
                        delete message;
                    } break;

                    case eSSCommand_Framework: {
                        LOG_WARN( "HarborSession::onMessage(Sid:{}) : drop the eSSCommand_Framework .\n", id() );
                        delete message;
                    } break;

                    default: {
                        if ( message->head.cmd == eSSCommand_Register ) {
                            static_cast<RegisterCommand *>(message)->endpoint = endpoint();
                        } else if ( message->head.cmd == eSSCommand_RegSucceed ) {
                            static_cast<RegSucceedCommand *>(message)->endpoint = endpoint();
                        }

                        // 其他命令直接提交到逻辑层
                        m_Service->POST<SSMessage>( iocontext(), eTaskType_SSMessage, message );
                    } break;
                }
            } break;
        }
    }

private:
    Harbor::NodeType m_Type;
    HarborService * m_Service;
    int32_t m_TimeoutSeconds;
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

HarborService::HarborService( const char * module, ISysConfigfile * sysconf, utils::IWorkThread * thread )
    : IIOService( sysconf->getFramework()->getThreadCount(), HOSTID_MAX * HOSTID_MAX, true ),
      m_HostID( sysconf->getServerID() ),
      m_HostType( sysconf->getFramework()->getType( m_HostID ) ),
      m_Module( module ),
      m_SysConfig( sysconf ),
      m_WorkThread( thread ),
      m_ProcessCallback( nullptr )
{}

HarborService::~HarborService()
{}

IIOSession * HarborService::onAccept( NetType type, uint16_t listenport, sid_t sid, const char * host, uint16_t port )
{
    // 线程安全
    return new HarborSession(
        Harbor::ACCEPT,
        this, m_SysConfig->getFramework()->getSessionTimeout() );
}

bool HarborService::onConnectFailed( int32_t result, const char * host, uint16_t port )
{
    LOG_ERROR( "HarborService::onConnectFailed(Reason:{:#010x}) : connect to the RemoteServer({}) failed .\n", result, Endpoint(host, port) );

    // 线程安全
    return m_SysConfig->getFramework()->has( Endpoint( host, port ) );
}

IIOSession * HarborService::onConnectSucceed( sid_t sid, const char * host, uint16_t port )
{
    LOG_INFO( "HarborService::onConnectSucceed(Sid:{}) : connect to the RemoteServer({}) succeed .\n", sid, Endpoint(host, port) );

    if ( !m_SysConfig->getFramework()->has( Endpoint( host, port ) ) ) {
        LOG_ERROR( "HarborService::onConnectSucceed(Sid:{}) : the RemoteServer({}) can't found in the Framework .\n", sid, Endpoint(host, port) );
        return nullptr;
    }

    // 线程安全
    return new HarborSession(
        Harbor::CONNECT,
        this, m_SysConfig->getFramework()->getSessionTimeout() );
}

void HarborService::process( int32_t type, void * task )
{
    if ( type != eTaskType_SSMessage ) {
        return;
    }

    SSMessage * msg = static_cast<SSMessage *>( task );

    switch ( msg->head.cmd ) {
        case eSSCommand_Register: {
            RegisterCommand * req = static_cast<RegisterCommand *>( msg );

            if ( m_HostType == HostType::Global
                || m_HostType == HostType::Master ) {
                // 向管理服务器注册
                register2(
                    req->sid, req->hosttype, req->zoneid, req->procmark, req->endpoint );
            } else {
                HostID hid = req->hostid;

                // 全局服的区服ID为0,
                // 全局服内部互联的时候, hid和主机ID是一致的
                if ( m_SysConfig->getZoneID() == 0 ) {
                    hid = Harbor::splice( req->zoneid, req->hostid );
                }

                sid_t sid = m_Harbor.sid( hid );
                if ( sid != 0 ) {
                    m_Harbor.del( sid );
                    shutdown( sid );
                }
                // 通知对端注册成功
                RegSucceedCommand resp;
                resp.zoneid = m_SysConfig->getZoneID();
                resp.hostid = m_HostID;
                resp.hosttype = m_HostType;
                resp.endpoint = *m_SysConfig->getEndpoint();
                send( req->sid, &resp );
                // 添加对端信息
                m_Harbor.add( Harbor::ACCEPT,
                    req->hosttype, hid, req->sid, req->endpoint );
                if ( m_ProcessCallback != nullptr ) m_ProcessCallback( hid, req );
                LOG_TRACE( "HarborService::process(Sid:{}) : the RemoteServer(HostType:{}, HostID:{}/{}, {}) is registered to ME .\n",
                    req->sid, req->hosttype, req->zoneid, req->hostid, req->endpoint );
            }
        } break;

        case eSSCommand_RegSucceed: {
            RegSucceedCommand * req = static_cast<RegSucceedCommand *>( msg );

            m_Harbor.add( Harbor::CONNECT,
                req->hosttype, req->hostid, req->sid, req->endpoint );
            if ( m_ProcessCallback != nullptr ) m_ProcessCallback( req->hostid,  req );
            LOG_TRACE( "HarborService::process(Sid:{}) : connect to the RemoteServer(HostType:{}, HostID:{}/{}, {}) succeed .\n",
                req->sid, req->hosttype, req->zoneid, req->hostid, req->endpoint );
        } break;

        case eSSCommand_Unregister: {
            UnregisterCommand * req = static_cast<UnregisterCommand *>( msg );

            HostID id = m_Harbor.hostid( req->sid );
            if ( id != 0 ) {
                auto [ zoneid, hostid ] = Harbor::split( id );
                if ( zoneid == 0 ) zoneid = m_SysConfig->getZoneID();
                if ( m_ProcessCallback != nullptr ) m_ProcessCallback( id,  req );
                m_Harbor.del( req->sid );
                LOG_TRACE( "HarborService::process(Sid:{}) : the HarborClient(HostID:{}/{}, {}) disconnect from ME .\n",
                    req->sid, zoneid, hostid, req->endpoint );
            }
        } break;

            // Slave过来的消息
        case eSSCommand_Framework: {
            FrameworkCommand * req = (FrameworkCommand *)msg;

            if ( m_HostType == HostType::Master ) {
                // 接收到Global下发的框架
                m_SysConfig->getFramework()->updateGlobalHosts( req->endpoints );
                // 通知所有客户端框架图变更
                broadcastFramework();
            } else {
                // 加载framework配置
                m_SysConfig->getFramework()->updateLocalHosts( req->endpoints );
                // harbor重连
                rebuild( m_SysConfig->getConnectList() );
            }

            if ( m_ProcessCallback != nullptr ) m_ProcessCallback( m_Harbor.hostid(req->sid),  req );
            LOG_DEBUG( "this Zone(ServerID:{}, ZoneID:{})'s Framework is \n{}",
                m_SysConfig->getServerID(), m_SysConfig->getZoneID(), m_SysConfig->getFramework()->getInformation() );
        } break;
    }
}

bool HarborService::connects( const HostTypes & types )
{
    if ( types.empty() ) {
        return false;
    }

    HostEndpoints eplist;

    for ( size_t i = 0; i < types.size(); ++i ) {
        m_SysConfig->getFramework()->getEndpoints( types[i], eplist );
    }

    // 发现需要互联的情况
    if ( types.end()
        != std::find( types.begin(), types.end(), m_HostType ) ) {
        for ( HostEndpoints::iterator it = eplist.begin(); it != eplist.end(); ) {
            if ( it->type != m_HostType || it->id < m_HostID ) {
                ++it;
            } else {
                it = eplist.erase( it );
            }
        }
    }

    // 建立连接
    for ( size_t i = 0; i < eplist.size(); ++i ) {
        connect( eplist[i].endpoint.host.c_str(), eplist[i].endpoint.port );
    }

    return true;
}

void HarborService::disconnects()
{
    // 获取所有连接
    sids_t sids;
    m_Harbor.sids( sids );

    // 关闭
    shutdown( sids );
}

bool HarborService::rebuild( const HostTypes & types )
{
    //
    sids_t shutdowns;
    HostIDs all, connects;

    // 获取所有需要连接的列表
    for ( const auto & type : types ) {
        m_SysConfig->getFramework()->getHostIDs( type, all );
    }

    // 与主机列表合并
    m_Harbor.merge( all, shutdowns, connects );

    // 需要关闭的服务器
    shutdown( shutdowns );

    // 尝试连接服务器
    for ( const auto & id : connects ) {
        Endpoint ep;

        if ( !m_SysConfig->getFramework()->getEndpoint( id, ep ) ) {
            continue;
        }

        if ( !isConnecting( ep.host.c_str(), ep.port ) ) {
            connect( ep.host.c_str(), ep.port );
        }
    }

    return true;
}

int32_t HarborService::send( sid_t sid, SSMessage * msg )
{
    Slice buf = msg->encode();

    if ( unlikely( buf.empty() ) ) {
        LOG_FATAL( "HarborSession::send(Sid:{}) : this Message(CMD:{:#06x}) encode() failed .\n", sid, msg->head.cmd );
        return -1;
    }

    // 发送
    return IIOService::send(
        sid, buf.data(), buf.size(), true );
}

int32_t HarborService::sendByHostID( HostID id, SSMessage * msg )
{
    return send( m_Harbor.sid( id ), msg );
}

int32_t HarborService::sendByRandom( HostType type, SSMessage * msg )
{
    HostID hid = m_Harbor.random( type );
    if ( hid == 0 ) {
        return -1;
    }

    return sendByHostID( hid, msg );
}

int32_t HarborService::broadcast( SSMessage * msg )
{
    int32_t result = 0;
    Slice buf = msg->encode();

    if ( unlikely( buf.empty() ) ) {
        LOG_FATAL( "HarborSession::broadcast() : this Message(CMD:{:#06x}) encode() failed .\n", msg->head.cmd );
        return 0;
    }

    result = IIOService::broadcast( buf.data(), buf.size() );
    std::free( (void *)buf.data() );

    return result;
}

int32_t HarborService::broadcast( HostType type, SSMessage * msg )
{
    // 取出所有网关的会话ID
    sids_t tolist;
    m_Harbor.sids( type, tolist );

    return broadcast( tolist, msg );
}

int32_t HarborService::broadcast( const sids_t & tolist, SSMessage * msg )
{
    if ( tolist.empty() ) {
        return 0;
    }

    int32_t result = 0;
    Slice buf = msg->encode();

    if ( unlikely( buf.empty() ) ) {
        LOG_FATAL( "HarborSession::broadcast() : this Message(CMD:{:#06x}) encode() failed .\n", msg->head.cmd );
        return 0;
    }

    result = IIOService::broadcast(
        tolist, buf.data(), buf.size() );
    std::free( (void *)buf.data() );

    return result;
}

int32_t HarborService::broadcast( const HostIDs & tolist, SSMessage * msg )
{
    if ( tolist.empty() ) {
        return 0;
    }

    int32_t result = 0;
    Slice buf = msg->encode();

    if ( unlikely( buf.empty() ) ) {
        LOG_FATAL( "HarborSession::broadcast() : this Message(CMD:{:#06x}) encode() failed .\n", msg->head.cmd );
        return 0;
    }

    for ( size_t i = 0; i < tolist.size(); ++i ) {
        IIOService::send(
            m_Harbor.sid( tolist[i] ), buf.data(), buf.size() );
    }
    std::free( (void *)buf.data() );

    return result;
}

void HarborService::broadcastFramework()
{
    FrameworkCommand command;

    // 超时时间
    command.threadcount = m_SysConfig->getFramework()->getThreadCount();
    command.sesstimeout = m_SysConfig->getFramework()->getSessionTimeout();
    // 广播其他服务器更新framework
    const HostEndpoints & hosts = m_SysConfig->getFramework()->getHostEndpoints();
    for ( size_t i = 0; i < hosts.size(); ++i ) {
        HostEndpoint h;
        h.id = hosts[i].id;
        h.type = hosts[i].type;
        h.endpoint = hosts[i].endpoint;
        h.extendpoint = hosts[i].extendpoint;
        command.endpoints.push_back( h );
    }

    broadcast( &command );
}

int32_t HarborService::sendRegisterCommand( sid_t sid, const Endpoint & ep )
{
    RegisterCommand command;
    command.hostid = m_HostID;
    command.hosttype = m_HostType;
    command.procmark = m_Module;
    command.endpoint = ep;
    command.zoneid = m_SysConfig->getZoneID();
    return send( sid, &command );
}

void HarborService::register2( sid_t sid, HostType type, ZoneID zoneid, const std::string & procmark, const Endpoint & ep )
{
    bool isvalid = false;
    FrameworkCommand command;

    // 主要信息
    command.pfid = m_SysConfig->getPfID();
    command.machineid = m_SysConfig->getMachineID();
    command.appid = m_SysConfig->getAppID();
    command.zoneid = m_SysConfig->getZoneID();
    command.zonemode = m_SysConfig->getZoneMode();
    command.starttime = m_SysConfig->getStartTime();
    command.mergezonelist = m_SysConfig->getMergeList();
    command.threadcount = m_SysConfig->getFramework()->getThreadCount();
    command.sesstimeout = m_SysConfig->getFramework()->getSessionTimeout();
    // 从framework中取出所有服务器列表
    for ( const auto & host : m_SysConfig->getFramework()->getHostEndpoints() ) {
        HostEndpoint h;
        h.id = host.id;
        h.type = host.type;
        h.endpoint = host.endpoint;
        h.extendpoint = host.extendpoint;
        command.endpoints.push_back( h );
        // 找到自己的id
        // 绑定了unix domain socket, 不需要校验远程IP
        if ( procmark == host.procmark
            && ( host.endpoint.port == 0 || ep.host == host.endpoint.host ) ) {
            isvalid = true;
            command.hostid = host.id;
            command.hosttype = host.type;
            command.connectlist = host.connectlist;
        }
    }

    // 返回, 避免Slave在傻等超时时间
    send( sid, &command );

    if ( !isvalid ) {
        shutdown( sid );
        LOG_ERROR( "HarborService::register2(HostType:{}, ProcessMarks:'{}') : register failed .\n", type, procmark );
    } else {
        // 判断hostid对应的sid是否存在
        sid_t cursid = m_Harbor.sid( command.hostid );
        if ( cursid == 0 ) {
            // 添加
            m_Harbor.add(
                Harbor::ACCEPT, command.hosttype, command.hostid, sid, ep );
            LOG_INFO( "HarborService::register2(HostType:{}, ProcessMarks:'{}') : the Server(HostID:{}/{}) register succeed .\n", type, procmark, zoneid, command.hostid );
        } else {
            shutdown( sid );
            LOG_ERROR( "HarborService::register2(HostType:{}, ProcessMarks:'{}') : this Server(HostID:{}/{}) register repeatedly .\n", type, procmark, zoneid, command.hostid );
        }
    }
}
