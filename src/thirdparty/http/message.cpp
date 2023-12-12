
#include <cstdio>
#include <cstring>

#include "utils/xtime.h"
#include "utils/utility.h"

#include "message.h"

const char * HttpMessage::HEADER_DATE = "Date";
const char * HttpMessage::HEADER_SERVER = "Server";
const char * HttpMessage::HEADER_KEEPALIVE = "Keep-Alive";
const char * HttpMessage::HEADER_CONNECTION = "Connection";
const char * HttpMessage::HEADER_CONTENT_TYPE = "Content-Type";
const char * HttpMessage::HEADER_CONTENT_LENGTH = "Content-Length";
const char * HttpMessage::HEADER_PROXY_CONNECTION = "Proxy-Connection";
const char * HttpMessage::HEADER_TRANSFER_ENCODING = "Transfer-Encoding";

HttpMessage::HttpMessage( int8_t type )
    : m_Type( type ),
      m_Version( "HTTP/1.1" ),
      m_Capacity( 0 )
{}

HttpMessage::~HttpMessage()
{}

void HttpMessage::printAll() const
{
    std::printf( "HttpMessage {\n" );
    std::printf( "\tVersion: %s\n", getVersion().c_str() );

    for ( const auto & value : m_Headers ) {
        std::printf( "\t%s: %s\n", value.first.c_str(), value.second.c_str() );
    }

    std::printf( "\tContent: %s\n", m_Content.c_str() );
    std::printf( "}\n" );
}

void HttpMessage::addHeader( const std::string & key, const std::string & value )
{
    m_Headers.emplace( key, value );
}

std::string HttpMessage::operator[]( const std::string & key ) const
{
    auto result = m_Headers.find( key );
    if ( result != m_Headers.end() ) {
        return result->second;
    }

    return "";
}

bool HttpMessage::isKeepalive() const
{
    auto result = m_Headers.find( HEADER_CONNECTION );
    if ( result != m_Headers.end() ) {
        if ( strcasecmp( result->second.c_str(), "close" ) == 0 ) {
            return false;
        } else if ( strcasecmp( result->second.c_str(), "keep-alive" ) == 0 ) {
            return true;
        }
    }

    // HTTP/1.0默认关闭, 其他版本默认keepalive
    return ( strcasecmp( m_Version.c_str(), "HTTP/1.0" ) == 0 ) ? false : true;
}

void HttpMessage::parseField( const std::string & value, Headers & fields )
{
    char *word, *brkt;
    const char * sep = ",";
    char * belist = strdup( value.c_str() );

    for ( word = strtok_r( belist, sep, &brkt );
          word;
          word = strtok_r( nullptr, sep, &brkt ) ) {
        char * v = word;
        char * k = utils::Utility::strsep( &v, "=" );
        if ( k != nullptr && v != nullptr ) {
            fields.emplace( k, v );
        }
    }

    std::free( belist );
}

////////////////////////////////////////////////////////////////////////////////

HttpRequest::HttpRequest()
    : HttpMessage( HttpMessage::eRequest )
{}

HttpRequest::~HttpRequest()
{}

void HttpRequest::printAll() const
{
    HttpMessage::printAll();

    std::printf( "HttpRequest {\n" );
    std::printf( "\tMethod: %s\n", m_Method.c_str() );
    std::printf( "\tURL: %s, URI: %s\n", m_URL.c_str(), m_URI.c_str() );

    for ( Parameters::const_iterator it = m_Parameters.begin(); it != m_Parameters.end(); ++it ) {
        std::printf( "\t%s: %s\n", it->first.c_str(), it->second.c_str() );
    }
    std::printf( "}\n" );
}

void HttpRequest::addParameter( const std::string & key, const std::string & value )
{
    m_Parameters.emplace( key, value );
}

std::string HttpRequest::operator[]( const std::string & key ) const
{
    auto result = m_Parameters.find( key );
    if ( result != m_Parameters.end() ) {
        return result->second;
    }

    return ( *static_cast<const HttpMessage *>( this ) )[key];
}

////////////////////////////////////////////////////////////////////////////////

std::map<int32_t, std::string> HttpResponse::Status = {
    { 100, "Continue" },
    { 101, "Switching Protocols" },
    { 200, "OK" },
    { 201, "Created" },
    { 202, "Accepted" },
    { 203, "Non-Authoritative Information" },
    { 204, "No Content" },
    { 205, "Reset Content" },
    { 400, "Bad Request" },
    { 401, "Unauthorized" },
    { 402, "Payment Required" },
    { 403, "Forbidden" },
    { 404, "Not Found" },
    { 500, "Internal Server Error" },
    { 501, "Not Implemented" },
    { 502, "Bad Gateway" },
    { 503, "Service Unavailable" },
    { 504, "Gateway Time-out" },
    { 505, "HTTP Version not supported" }
};

HttpResponse::HttpResponse()
    : HttpMessage( HttpMessage::eResponse )
{}

HttpResponse::HttpResponse( const std::string & version )
    : HttpMessage( HttpMessage::eResponse ),
      m_HttpdVersion( version )
{}

HttpResponse::~HttpResponse()
{}

void HttpResponse::setResult( int32_t code )
{
    setStatusCode( code );
    setReasonPhrase( Status[code].c_str() );
}

void HttpResponse::addDefaultHeaders()
{
    char buf[1024];

    // 强行覆盖ServerHeader
    std::snprintf( buf, 1023, "YuYuHttpd-%s", m_HttpdVersion.c_str() );
    addHeader( HEADER_SERVER, buf );

    // HTTPDate
    if ( !hasHeader( HEADER_DATE ) ) {
        addHeader( HEADER_DATE,
                utils::TimeUtils().getFormatTime("%a, %d %b %Y %H:%M:%S %Z")  );
    }

    // ContentType
    if ( !m_Content.empty() && !hasHeader( HEADER_CONTENT_TYPE ) ) {
        addHeader( HEADER_CONTENT_TYPE, "text/html; charset=UTF-8" );
    }
}

void HttpResponse::serialize( std::string & buffer )
{
    char buf[1024];

    // StartLine
    std::snprintf( buf, 1023, "%s %i %s\r\n", getVersion().c_str(), getStatusCode(), getReasonPhrase().c_str() );
    buffer += buf;

    // HEADS
    // 添加默认头参数
    addDefaultHeaders();
    // 序列化
    for ( const auto & head : m_Headers ) {
        std::snprintf( buf, 1023, "%s: %s\r\n", head.first.c_str(), head.second.c_str() );
        buffer += buf;
    }
    // 未添加ContentLength的HTTP头
    if ( !m_Content.empty() && !hasHeader( HEADER_CONTENT_LENGTH ) ) {
        std::snprintf( buf, 1023, "%s: %lu\r\n", HEADER_CONTENT_LENGTH, m_Content.size() );
        buffer += buf;
    }
    // HEAD结束符
    buffer += "\r\n";

    // CONTENT
    buffer += m_Content;
}
