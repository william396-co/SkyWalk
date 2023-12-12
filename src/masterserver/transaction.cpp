
#include "base/types.h"
#include "base/base.h"
#include "base/result.h"

#include "utils/json.h"
#include "utils/utility.h"

#include "http/message.h"
#include "extra/httpsvc.h"
#include "message/gmprotocol.h"

#include "version.h"
#include "server.h"
#include "handler.h"

#include "transaction.h"

namespace master {

void ServerStatusTransaction::onTimeout()
{
    if ( m_HostID == 0 ) {
        // 返回Chunked
        std::string buffer( "0\r\n\r\n" );
        // 返回http请求
        g_HttpService->send( m_Sid, buffer );
    } else {
        g_HttpService->sendErrorcode( m_Sid,
            m_Version, m_Connection, __BUILD_VERSION__, eHttpResult_Timeout );
    }
}

void ServerStatusTransaction::onTrigger( void * data )
{
    // 返回的数据发送给gm工具
    sendStatus(
        static_cast<ServerStatusCommand *>( data )->id,
        static_cast<ServerStatusCommand *>( data )->type,
        static_cast<ServerStatusCommand *>( data )->status );
}

void ServerStatusTransaction::sendStatus( HostID id, HostType type, const std::string & status )
{
    // 返回给客户端
    utils::JsonWriter writer;
    writer.start();
    writer.put( "serverid", id );
    writer.put( "servertype", (int32_t)type );
    writer.put( "status", status );
    writer.end();

    if ( m_HostID != 0 ) {
        g_HttpService->sendContent( m_Sid,
            m_Version, m_Connection, __BUILD_VERSION__, writer.tostring() );
    } else {
        std::string buffer;
        // 返回Chunked
        utils::Utility::snprintf(
            buffer, writer.length() + 1024, "%x\r\n%s\r\n", writer.length(), writer.content() );
        g_HttpService->send( m_Sid, buffer );
    }
}

void ResultTransaction::onTimeout()
{
    g_HttpService->sendErrorcode( m_Sid,
            m_Version, m_Connection, __BUILD_VERSION__, eHttpResult_Timeout );

    if ( GameApp::instance().getReloadTransid() == getTransID() ) {
        GameApp::instance().setReloadTransid( 0 );
    }

    LOG_ERROR( "ResultTransaction::onTimeout(Version:{} tranid:{}) .\n", m_Version, getTransID() );
}

void ResultTransaction::onTrigger( void * data )
{
    g_HttpService->sendContent( m_Sid,
            m_Version, m_Connection, __BUILD_VERSION__, static_cast<GMResultCommand *>(data)->result );
}

void QueryRoleTransaction::onTimeout()
{
    g_HttpService->sendErrorcode( m_Sid,
            m_Version, m_Connection, __BUILD_VERSION__, eHttpResult_Timeout );
    LOG_ERROR( "QueryRoleTransaction::onTimeout(Version:{} tranid:{}) .\n", m_Version, getTransID() );
}

void QueryRoleTransaction::onTrigger( void * data )
{
    QueryRoleCommand * resp = (QueryRoleCommand *)data;

    StreamBuf pack(4096);
    pack.encode( resp->roleid );
    pack.encode( resp->tablename );
    pack.encode( resp->result );
    pack.encode( resp->records );
    g_HttpService->sendContent( m_Sid,
            m_Version, m_Connection, __BUILD_VERSION__, pack.string(), "application/streambuf" );
}

} // namespace master
