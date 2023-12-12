
#include <cstring>
#include <cstdlib>
#include <cassert>

#include "http/url.h"
#include "utils/utility.h"

#include "message.h"
#include "parser.h"

HttpParser::HttpParser()
    : m_Status( eStartLine ),
      m_HttpMessage( nullptr )
{}

HttpParser::~HttpParser()
{}

void HttpParser::reset()
{
    if ( m_HttpMessage != nullptr ) {
        delete m_HttpMessage;
        m_HttpMessage = nullptr;
    }

    m_Status = eStartLine;
}

bool HttpParser::isCompleted() const
{
    return m_Status == eCompleted;
}

HttpRequest * HttpParser::getRequest()
{
    HttpRequest * request = nullptr;

    if ( m_HttpMessage == nullptr ) {
        return nullptr;
    }

    if ( m_HttpMessage->getType() != HttpMessage::eRequest ) {
        return nullptr;
    }

    request = static_cast<HttpRequest *>( m_HttpMessage );
    m_HttpMessage = nullptr;

    return request;
}

HttpResponse * HttpParser::getResponse()
{
    HttpResponse * response = nullptr;

    if ( m_HttpMessage == nullptr ) {
        return nullptr;
    }

    if ( m_HttpMessage->getType() != HttpMessage::eResponse ) {
        return nullptr;
    }

    response = static_cast<HttpResponse *>( m_HttpMessage );
    m_HttpMessage = nullptr;

    return response;
}

int32_t HttpParser::append( const char * buffer, uint32_t length )
{
    uint32_t nparsed = 0;

    if ( m_Status == eCompleted ) {
        return nparsed;
    }

    if ( m_HttpMessage == nullptr ) {
        // 解析StartLine
        nparsed = this->parseStartLine( buffer, length );
    }

    if ( m_HttpMessage != nullptr ) {
        // 处理HTTP包头
        for ( int32_t n = 1;
              m_Status == eHeader && n > 0 && nparsed < length;
              nparsed += n ) {
            n = this->parseHeader( buffer + nparsed, length - nparsed );
        }

        // 处理HTTP内容
        if ( m_Status == eContent ) {
            // 处理普通内容以及Chunked内容
            if ( m_HttpMessage->hasHeader( HttpMessage::HEADER_TRANSFER_ENCODING )
                && strcasecmp( ( *m_HttpMessage )[HttpMessage::HEADER_TRANSFER_ENCODING].c_str(), "chunked" ) ) {
                nparsed += this->parseChunked( buffer + nparsed, length - nparsed );
            } else {
                nparsed += this->parseContent( buffer + nparsed, length - nparsed );
            }
        }

        // 处理POST参数
        if ( m_Status == eCompleted && m_HttpMessage->getType() == HttpMessage::eRequest ) {
            HttpParser::processPostContent( static_cast<HttpRequest *>( m_HttpMessage ) );
        }
    }

    return nparsed;
}

// 解析StartLine
uint32_t HttpParser::parseStartLine( const char * buffer, uint32_t length )
{
    std::string line;

    if ( !HttpParser::getline( std::string( buffer, length ), line ) ) {
        return 0;
    }

    char * pos = (char *)line.c_str();
    char * first = utils::Utility::strsep( &pos, " " );
    char * second = utils::Utility::strsep( &pos, " " );

    if ( std::strncmp( (char *)line.c_str(), "HTTP", 4 ) != 0 ) {
        // HTTP请求
        HttpRequest * request = new HttpRequest;

        if ( first != nullptr ) {
            request->setMethod( first );
        }
        if ( second != nullptr ) {
            request->setURL( second );
            request->setURI( utils::Utility::strsep( &second, "?" ) );
        }
        if ( pos != nullptr ) {
            request->setVersion( std::strtok( pos, "\r\n" ) );
        }

        // 参数列表
        if ( second != nullptr ) {
            HttpParser::parsePostParams( request, second );
        }

        m_Status = eHeader;
        m_HttpMessage = request;
    } else {
        // HTTP回应
        HttpResponse * response = new HttpResponse;

        if ( first != nullptr ) {
            response->setVersion( first );
        }
        if ( second != nullptr ) {
            response->setStatusCode( atoi( second ) );
        }
        if ( pos != nullptr ) {
            response->setReasonPhrase( strtok( pos, "\r\n" ) );
        }

        m_Status = eHeader;
        m_HttpMessage = response;
    }

    return line.size();
}

// 解析Header
uint32_t HttpParser::parseHeader( const char * buffer, uint32_t length )
{
    std::string line;

    if ( !HttpParser::getline( std::string( buffer, length ), line ) ) {
        return 0;
    }

    char * pos = (char *)line.c_str();
    char * key = utils::Utility::strsep( &pos, ":" );

    if ( pos != nullptr && key != nullptr ) {
        char * value = std::strtok( pos, "\r\n" );
        if ( value != nullptr ) {
            value += std::strspn( value, " " );
            m_HttpMessage->addHeader( key, value );
        }
    }

    if ( *buffer == '\r' || *buffer == '\n' ) {
        m_Status = eContent;
    }

    return line.size();
}

// 解析Chunked报文
uint32_t HttpParser::parseChunked( const char * buffer, uint32_t length )
{
    bool done = false;
    uint32_t nparsed = 0;

    while ( !done && m_Status != eCompleted ) {
        std::string blocksize;

        // 获取Chunked的block长度
        if ( !HttpParser::getline(
                 std::string( buffer + nparsed, length - nparsed ), blocksize, false ) ) {
            break;
        }

        uint32_t contentlen = 0;

        done = true;
        contentlen = std::strtol( blocksize.c_str(), nullptr, 16 );

        if ( contentlen > 0 && ( length - nparsed ) > ( contentlen + blocksize.size() ) ) {
            // 一块数据收完整了
            std::string block;
            uint32_t offset = nparsed + blocksize.size() + contentlen;

            if ( HttpParser::getline( std::string( buffer + offset, length - offset ), block ) ) {
                nparsed += blocksize.size();
                m_HttpMessage->setCapacity( contentlen );
                m_HttpMessage->appendContent( std::string( buffer + nparsed, contentlen ) );
                nparsed += ( contentlen + block.size() );
                done = false;
            }
        }

        if ( contentlen == 0 && blocksize.size() > 2 ) {
            m_Status = eCompleted;
            nparsed += blocksize.size();
        }
    }

    return nparsed;
}

// 解析常规内容
uint32_t HttpParser::parseContent( const char * buffer, uint32_t length )
{
    uint32_t nparsed = 0;
    uint32_t contentlen = 0;

    if ( m_HttpMessage->hasHeader( HttpMessage::HEADER_CONTENT_LENGTH ) ) {
        contentlen = std::atoi( ( *m_HttpMessage )[HttpMessage::HEADER_CONTENT_LENGTH].c_str() );
    }

    if ( contentlen > 0 && length >= contentlen ) {
        m_HttpMessage->setCapacity( contentlen );
        m_HttpMessage->appendContent( std::string( buffer, contentlen ) );
        nparsed += contentlen;
    }

    if ( contentlen == m_HttpMessage->getContentCapacity() ) {
        m_Status = eCompleted;
    }

    return nparsed;
}

bool HttpParser::getline( const std::string & buffer, std::string & line, bool keeplineendings )
{
    const void * buf = buffer.data();
    uint32_t length = buffer.size();

    char * pos = (char *)std::memchr( buf, '\n', length );
    if ( pos == nullptr ) {
        return false;
    } else if ( *( pos - 1 ) != '\r' ) {
        return false;
    }

    if ( keeplineendings ) {
        line.assign( buffer.c_str(),
            pos - (char *)buffer.c_str() + 1 );
    } else {
        line.assign( buffer.c_str(),
            pos - (char *)buffer.c_str() - 1 );
    }

    return true;
}

void HttpParser::processPostContent( HttpRequest * request )
{
    if ( request->getContentSize() == 0 ) {
        return;
    }

    HttpMessage * message = (HttpMessage *)request;

    // 内容的类型是POST才行
    if ( message->hasHeader( HttpMessage::HEADER_CONTENT_TYPE )
        && strcasecmp(
               "application/x-www-form-urlencoded",
               ( *message )[HttpMessage::HEADER_CONTENT_TYPE].c_str() )
            != 0 ) {
        return;
    }

    // 解析POST参数
    HttpParser::parsePostParams( request, (char *)request->getContent().c_str() );
}

void HttpParser::parsePostParams( HttpRequest * request, char * params )
{
    std::string p = params;
    char * buffer = (char *)p.data();

    for ( ; buffer != nullptr && *buffer != '\0'; ) {
        char * value = utils::Utility::strsep( &buffer, "&" );
        char * key = utils::Utility::strsep( &value, "=" );

        if ( key != nullptr ) {
            if ( value == nullptr ) {
                request->addParameter( key, "" );
            } else {
                std::string v;
                Url::decode( std::string( value ), v );
                request->addParameter( key, v.c_str() );
            }
        }
    }
}
