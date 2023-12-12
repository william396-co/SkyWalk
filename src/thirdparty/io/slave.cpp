
#include <chrono>
#include <unistd.h>

#include "base/base.h"

#include "utils/types.h"
#include "utils/thread.h"
#include "utils/utility.h"
#include "utils/streambuf.h"

#include "message/protocol.h"
#include "message/gateprotocol.h"

#include "slave.h"
#include "framework.h"

class SlaveConnection : public IIOSession
{
public:
    SlaveConnection( Slave * slave )
        : m_Slave( slave )
    {}

    virtual ~SlaveConnection()
    {}

    virtual int32_t onStart()
    {
        // 超时时间等到framework下发后设置

        // 断线重连后, 自动注册消息
        if ( m_Slave->hostid() != 0
            && m_Slave->hosttype() != HostType::None ) {
            m_Slave->sendRegisterCommand(
                m_Slave->hostid(), m_Slave->hosttype() );
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
        LOG_WARN( "SlaveConnection::onTimeout(Sid:{}, {}) .\n", id(), endpoint() );
        return 0;
    }

    virtual int32_t onKeepalive()
    {
        PingCommand cmd;
        send( &cmd );
        return 0;
    }

    virtual int32_t onError( int32_t result )
    {
        LOG_ERROR( "SlaveConnection::onError(Sid:{}, {}): {:#010x} .\n", id(), endpoint(), result );
        return 0;
    }

    virtual void onShutdown( int32_t way )
    {
        LOG_INFO( "SlaveConnection::onShutdown(Sid:{}, {}): way={} .\n", id(), endpoint(), way );
    }

private:
    // 发送
    int32_t send( SSMessage * msg )
    {
        Slice buf = msg->encode();
        if ( unlikely( buf.empty() ) ) {
            LOG_FATAL( "SlaveConnection::send(Sid:{}) : this Message(CMD:{:#06x}) encode() failed .\n", id(), msg->head.cmd );
            return -1;
        }

        return IIOSession::send(
            buf.data(), buf.size(), true );
    }

    // 完整的逻辑包
    void onMessage( SSMessage * message )
    {
        if ( message->head.type == eMessageType_Command ) {
            if ( message->head.cmd == eSSCommand_Ping ) {
                delete message;
                message = nullptr;
            } else if ( message->head.cmd == eSSCommand_Framework ) {
                // 更新超时时间
                FrameworkCommand * cmd = (FrameworkCommand *)message;
                setTimeout( cmd->sesstimeout );
                setKeepalive( cmd->sesstimeout / 5 );

                // 更新框架信息
                if ( m_Slave->hostid() == 0 ) {
                    m_Slave->updateFramework( message );
                    delete message;
                    message = nullptr;
                }
            }
        }

        if ( message != nullptr ) {
            if ( !m_Slave->post( eTaskType_SSMessage, message ) ) {
                delete message;
            }
        }
    }

private:
    Slave * m_Slave;
};

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

Slave::Slave()
    : IIOService( 1, 32 ),
      m_HostID( 0 ),
      m_HostType( HostType::None ),
      m_ClientSid( 0ULL ),
      m_SysConf( nullptr ),
      m_WorkThread( nullptr )
{}

Slave::~Slave()
{}

bool Slave::onConnectFailed( int32_t result, const char * host, uint16_t port )
{
    LOG_ERROR( "Slave::onConnectFailed(Reason:{:#010x}) : connect to the MasterServer({}) failed .\n", result, Endpoint(host, port) );
    return true;
}

IIOSession * Slave::onConnectSucceed( sid_t sid, const char * host, uint16_t port )
{
    return new SlaveConnection( this );
}

bool Slave::initialize( const char * module,
    HostType type, const Endpoint & ep,
    utils::IWorkThread * thread, ISysConfigfile * sysconf, int32_t timeout )
{
    m_Module = module;
    m_SysConf = sysconf;
    m_WorkThread = thread;

    if ( !start() ) {
        LOG_FATAL( "Slave::initialize() : start() failed .\n" );
        return false;
    }

    m_ClientSid = connect( ep.host.c_str(), ep.port, timeout );
    if ( m_ClientSid == (sid_t)-1 ) {
        LOG_FATAL( "Slave::initialize() : connect 2 the MasterServer({}) failed .\n", ep );
        return false;
    }

    // 发送注册消息
    sendRegisterCommand( getHostID( type ), type );

    // 等待
    if ( timeout > 0 ) {
        // 计算超时时间
        std::unique_lock<std::mutex> lk( m_Lock );
        m_Cond.wait_for( lk,
            std::chrono::seconds( timeout ),
            [this]() {
                return m_HostID != 0 || m_HostID == (HostID)-1;
            } );
    }

    return m_HostID != 0 && m_HostID != (HostID)-1;
}

void Slave::finalize()
{
    halt();
    stop();
}

HostID Slave::hostid()
{
    std::lock_guard lock( m_Lock );
    return m_HostID;
}

HostType Slave::hosttype()
{
    std::lock_guard lock( m_Lock );
    return m_HostType;
}

HostID Slave::getHostID( HostType type ) const
{
    HostIDs hosts;
    m_SysConf->getFramework()->getHostIDs( type, hosts );

    if ( hosts.empty() ) {
        return 0;
    }

    return *( hosts.begin() );
}

void Slave::updateFramework( SSMessage * message )
{
    FrameworkCommand * cmd = (FrameworkCommand *)message;
    {
        std::lock_guard lock( m_Lock );

        // 设置主机ID和主机类型
        m_HostType = cmd->hosttype;
        m_HostID = cmd->hostid == 0 ? -1 : cmd->hostid;

        // Master中会填充跨服服务器信息
        if ( cmd->hosttype == HostType::Master ) {
            // 填充跨服信息
            m_SysConf->getFramework()->updateGlobalHosts( cmd->endpoints );
        } else {
            // 设置framework
            m_SysConf->setServerID( cmd->hostid );
            m_SysConf->setPfID( cmd->pfid );
            m_SysConf->setMachineID( cmd->machineid );
            m_SysConf->setAppID( cmd->appid );
            m_SysConf->setZoneID( cmd->zoneid );
            m_SysConf->setStartTime( cmd->starttime );
            m_SysConf->setZoneMode( cmd->zonemode );
            m_SysConf->setMergeList( cmd->mergezonelist );
            m_SysConf->setConnectList( cmd->connectlist );
            m_SysConf->getFramework()->setSessTimeout( cmd->sesstimeout );
            m_SysConf->getFramework()->setThreadCount( cmd->threadcount );
            m_SysConf->getFramework()->updateLocalHosts( cmd->endpoints );
        }
    }
    // 通知逻辑层
    m_Cond.notify_all();

    // 打印调试信息
    LOG_INFO( "the Slave get this Zone(ZoneID:{}, ServerID:{})'s Framework :\n{}",
        m_SysConf->getZoneID(),
        m_SysConf->getServerID(),
        m_SysConf->getFramework()->getInformation() );
}

bool Slave::post( int32_t type, void * task )
{
    if ( m_WorkThread == nullptr
        || type != eTaskType_SSMessage ) {
        return false;
    }

    return m_WorkThread->post( eTaskType_SSMessage, task );
}

void Slave::sendRegisterCommand( HostID id, HostType type, const Endpoint & ep )
{
    //
    // 进程标识定义:
    // [工作目录]/[可执行文件]#<实例号>
    //
    // 简化模式
    // 如果工作目录和可执行文件所在目录一致的情况下, 简化为
    // [可执行文件]#<实例号>
    //
    char cwdpath[512];
    std::string dirpath, procmark = m_Module;

    ::getcwd( cwdpath, 511 );
    {
        std::string exepath, basename;
        utils::Utility::getExePath( exepath );
        utils::PathUtils( exepath ).split( dirpath, basename );
    }
    procmark = cwdpath;
    procmark += "/";
    procmark += m_Module;

    RegisterCommand message;
    message.hostid = id;
    message.hosttype = type;
    message.procmark = procmark;
    message.endpoint = ep;
    message.zoneid = m_SysConf->getZoneID();
    send( &message );
}

int32_t Slave::send( SSMessage * msg )
{
    Slice buf = msg->encode();
    if ( unlikely( buf.empty() ) ) {
        LOG_FATAL( "Slave::send(Sid:{}) : this Message(CMD:{:#06x}) encode() failed .\n", m_ClientSid, msg->head.cmd );
        return -1;
    }

    // 发送
    return IIOService::send(
        m_ClientSid, buf.data(), buf.size(), true );
}
