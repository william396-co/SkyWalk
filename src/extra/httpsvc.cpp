
#include "io/task.h"
#include "base/base.h"
#include "utils/xtime.h"
#include "utils/thread.h"

#include "http/parser.h"
#include "http/message.h"

#include "httpsvc.h"
#include "utils/json.h"

class HttpSession : public IIOSession
{
public:
    HttpSession( utils::IWorkThread * thread, int32_t keepalive )
        : m_Parser( nullptr ),
          m_AppThread( thread ),
          m_Keepalive( keepalive )
    {}

    virtual ~HttpSession()
    {}

    virtual int32_t onStart()
    {
        m_Parser = new HttpParser;
        if ( m_Parser == nullptr ) {
            return -1;
        }

        if ( m_Keepalive > 0 ) {
            setTimeout( m_Keepalive );
        }

        return 0;
    }

    virtual ssize_t onProcess( const char * buffer, size_t nbytes )
    {
        size_t nparsed = 0;

        while ( nparsed < nbytes ) {
            nparsed += m_Parser->append(
                buffer + nparsed, nbytes - nparsed );

            // 判断消息是否解析完毕
            if ( !m_Parser->isCompleted() ) {
                break;
            }

            // 获得完整的HTTP请求
            HttpRequest * request = m_Parser->getRequest();
            if ( request != nullptr ) {
                process( request );
            }

            m_Parser->reset();
        }

        return nparsed;
    }

    virtual int32_t onTimeout()
    {
        LOG_WARN( "HttpSession::onTimeout(Sid:{}, {}) .\n", id(), endpoint() );
        return -1;
    }

    virtual int32_t onError( int32_t result )
    {
        LOG_DEBUG( "HttpSession::onError(Sid:{}, {}): {:#010x} .\n", id(), endpoint(), result );
        return -1;
    }

    virtual void onShutdown( int32_t way )
    {
        LOG_TRACE( "HttpSession::onShutdown(Sid:{}, {}): way={} .\n", id(), endpoint(), way );
        if ( m_Parser != nullptr ) {
            delete m_Parser;
            m_Parser = nullptr;
        }
    }

    void process( HttpRequest * request )
    {
        request->printAll();

        // NOTICE: 添加真实IP
        // NOTICE: Cloudflare, True-Client-IP
        // https://support.cloudflare.com/hc/en-us/articles/200170986-How-does-Cloudflare-handle-HTTP-Request-headers-
        if ( request->hasHeader( "True-Client-IP" ) ) {
            request->addHeader( "GAME_REAL_IP",
                ( *request )["True-Client-IP"].c_str() );
        } else {
            request->addHeader( "GAME_REAL_IP", host().c_str() );
        }

        std::string value = ( *request )[HttpMessage::HEADER_KEEPALIVE];
        if ( !value.empty() ) {
            // timeout=5, max=1000
            HttpMessage::Headers fields;
            request->parseField( value, fields );

            auto result = fields.find( "timeout" );
            if ( result != fields.end() ) {
                int32_t timeout = std::atoi( result->second.c_str() );
                if ( timeout > 0 ) {
                    // 设置时间
                    setTimeout( std::min( timeout, m_Keepalive ) );
                }
            }
        }

        // 给adminserver提交请求
        if ( !m_AppThread->post(
                 eTaskType_HttpMessage, new HttpRequestTask( id(), request ) ) ) {
            delete request;
        }
    }

private:
    HttpParser * m_Parser;
    utils::IWorkThread * m_AppThread;
    int32_t m_Keepalive;
};

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

HttpService::HttpService( uint8_t nthreads, uint32_t nclients, uint32_t capacity )
    : IIOService( nthreads, nclients ),
      m_AppThread( nullptr ),
      m_KeepaliveSeconds( -1 ),
      m_AccessFilter( capacity )
{}

HttpService::~HttpService()
{}

IIOSession * HttpService::onAccept( NetType type, uint16_t listenport, sid_t id, const char * host, uint16_t port )
{
    return new HttpSession( m_AppThread, m_KeepaliveSeconds );
}

bool HttpService::isRestricted( const std::string & account, int32_t count )
{
    std::pair<int32_t, int64_t> result;

    if ( !m_AccessFilter.take( account, result ) ) {
        result.first = 0;
        result.second = utils::now();
    }

    if ( ++result.first > count ) {
        return utils::now() - result.second < 1000;
    }

    m_AccessFilter.put( account, result );

    return false;
}

bool HttpService::sendErrorcode( sid_t sid, HttpRequest * request, const std::string & server, int32_t result )
{
    return sendErrorcode( sid, request->getVersion(), ( *request )[HttpMessage::HEADER_CONNECTION], server, result );
}

bool HttpService::sendErrorcode( sid_t sid, const std::string & version, const std::string & connection, const std::string & server, int32_t result )
{
    utils::JsonWriter writer;
    writer.start();
    writer.put( "result", result );
    writer.end();
    return sendContent( sid, version, connection, server, writer.tostring() );
}

bool HttpService::sendContent( sid_t sid, HttpRequest * request, const std::string & server, const std::string & content, const std::string & contenttype )
{
    return sendContent( sid, request->getVersion(), ( *request )[HttpMessage::HEADER_CONNECTION], server, content, contenttype );
}

bool HttpService::sendContent( sid_t sid, const std::string & version, const std::string & connection, const std::string & server, const std::string & content, const std::string & contenttype )
{
    HttpResponse response( server );
    response.setResult( 200 );
    response.setVersion( version );
    response.addHeader( HttpMessage::HEADER_CONNECTION, connection );
    response.addHeader( HttpMessage::HEADER_CONTENT_TYPE, contenttype );
    response.setCapacity( content.size() );
    response.appendContent( content );

    std::string buffer;
    response.serialize( buffer );
    send( sid, buffer );

    return true;
}
