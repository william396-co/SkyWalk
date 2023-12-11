
#include "base/base.h"

#include "utils/xtime.h"
#include "utils/streambuf.h"

#include "agentimpl.h"
#include "resultqueue.h"

#include "agentclient.h"

namespace data {

AgentSession::AgentSession( AgentClient * c )
    : m_AgentClient( c )
{}

AgentSession::~AgentSession()
{}

int32_t AgentSession::onStart()
{
    LOG_DEBUG( "data::AgentSession::onStart(Sid:{}, {}) .\n", id(), endpoint() );

    // 设置超时
    setTimeout( m_AgentClient->timeout() );
    // 设置KEEPALIVE
    setKeepalive( m_AgentClient->timeout() / 5 );

    DBHandShakeCommand cmd;
    cmd.version = m_AgentClient->getAgent()->schemeversion();
    m_AgentClient->send( id(), &cmd );
    m_AgentClient->post( eTaskType_SSMessage, new RegisterCommand( id() ) );

    return 0;
}

ssize_t AgentSession::onProcess( const char * buffer, size_t nbytes )
{
    ssize_t nprocess = 0;

    for ( ;; ) {
        size_t nleft = nbytes - nprocess;
        const char * buf = buffer + nprocess;

        if ( nleft < sizeof( SSHead ) ) {
            break;
        }

        SSHead head;
        StreamBuf unpack( buf, nleft );
        unpack.decode( head.type );
        unpack.decode( head.flags );
        unpack.decode( head.cmd );
        unpack.decode( head.size );

        if ( nleft < head.size + sizeof( SSHead ) ) {
            break;
        }

        SSMessage * msg = GeneralDecoder::instance().decode(
            id(), head, Slice( buf + sizeof( SSHead ), head.size ) );
        if ( msg != nullptr ) {
            m_AgentClient->post( eTaskType_SSMessage, msg );
        }

        nprocess += ( head.size + sizeof( SSHead ) );
    }

    return nprocess;
}

int32_t AgentSession::onTimeout()
{
    m_AgentClient->post( eTaskType_SSMessage,
        new UnregisterCommand( id(), endpoint() ) );
    LOG_ERROR( "data::AgentSession::onTimeout(Sid:{}, {}) .\n", id(), endpoint() );
    return 0;
}

int32_t AgentSession::onKeepalive()
{
    PingCommand cmd;
    m_AgentClient->send( id(), &cmd );
    return 0;
}

int32_t AgentSession::onError( int32_t result )
{
    m_AgentClient->post( eTaskType_SSMessage,
        new UnregisterCommand( id(), Endpoint( host(), port() ) ) );
    LOG_ERROR( "data::AgentSession::onError(Sid:{}, {}) : {:#010x} .\n", id(), endpoint(), result );
    return 0;
}

void AgentSession::onShutdown( int32_t way )
{
    LOG_TRACE( "data::AgentSession::onShutdown(Sid:{}, {}) : way={} .\n", id(), endpoint(), way );
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

AgentClient::AgentClient( DataAgentImpl * agent )
    : IIOService( 1, 32, true ),
      m_DataAgent( agent )
{}

AgentClient::~AgentClient()
{}

bool AgentClient::onConnectFailed( int32_t result, const char * host, uint16_t port )
{
    LOG_FATAL( "data::AgentClient::onConnectFailed() : connect 2 DataServer({}) failed {:#06x} .\n", Endpoint(host, port), result );
    return true;
}

IIOSession * AgentClient::onConnectSucceed( sid_t id, const char * host, uint16_t port )
{
    return new AgentSession( this );
}

uint32_t AgentClient::timeout() const
{
    return m_DataAgent->getTimeoutSeconds();
}

bool AgentClient::send( sid_t sid, SSMessage * message )
{
    // 序列化
    Slice buf = message->encode();

    // 发送
    return IIOService::send(
        sid, buf.data(), buf.size(), true );
}

void AgentClient::post( int32_t type, void * task )
{
    m_DataAgent->getResultQueue()->post( type, task );
}

} // namespace data
