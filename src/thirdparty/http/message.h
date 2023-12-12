
#ifndef __SRC_HTTP_MESSAGE_H__
#define __SRC_HTTP_MESSAGE_H__

#include <map>
#include <string>

#include <stdint.h>

// HTTP消息
class HttpMessage
{
public:
    static const char * HEADER_DATE;
    static const char * HEADER_SERVER;
    static const char * HEADER_KEEPALIVE;
    static const char * HEADER_CONNECTION;
    static const char * HEADER_CONTENT_TYPE;
    static const char * HEADER_CONTENT_LENGTH;
    static const char * HEADER_PROXY_CONNECTION;
    static const char * HEADER_TRANSFER_ENCODING;
    enum { eRequest, eResponse };
    typedef std::map<std::string, std::string> Headers;

public:
    HttpMessage( int8_t type );
    virtual ~HttpMessage();
    virtual void printAll() const;

public:
    // 类型
    int8_t getType() const { return m_Type; }

    // VERSION
    void setVersion( const std::string & v ) { m_Version = v; }
    const std::string & getVersion() const { return m_Version; }

    // HEADER
    // 添加Header
    void addHeader( const std::string & key, const std::string & value );
    // 获取所有包头
    const Headers & getHeaders() const { return m_Headers; }
    // 获取包头对应的VALUE
    std::string operator[]( const std::string & key ) const;
    // 是否有这个参数
    bool hasHeader( const std::string & header ) const { return m_Headers.end() != m_Headers.find( header ); }

    // CONTENT
    // 设置容量
    void setCapacity( uint32_t c )
    {
        m_Capacity = c;
        m_Content.reserve( c );
    }
    // 获取内容
    const std::string & getContent() const { return m_Content; }
    // 获取内容当前长度
    uint32_t getContentSize() const { return m_Content.size(); }
    // 获取内容总长度
    uint32_t getContentCapacity() const { return m_Capacity; }
    // 添加内容
    void appendContent( const std::string & value ) { m_Content += value; }

public:
    // 是否保活
    bool isKeepalive() const;
    // 解析
    void parseField( const std::string & value, Headers & fields );

protected:
    int8_t m_Type;         // 类型
    std::string m_Version; // 版本号
    Headers m_Headers;     // HTTP头
    uint32_t m_Capacity;   // 容量
    std::string m_Content; // 内容
};

// HTTP请求
class HttpRequest : public HttpMessage
{
public:
    typedef std::map<std::string, std::string> Parameters;
    HttpRequest();
    virtual ~HttpRequest();
    virtual void printAll() const;

public:
    // URL
    void setURL( const char * url ) { m_URL = url; }
    const std::string & getURL() const { return m_URL; }

    // URI
    void setURI( const char * uri ) { m_URI = uri; }
    const std::string & getURI() const { return m_URI; }

    // METHOD
    void setMethod( const char * method ) { m_Method = method; }
    const std::string & getMethod() const { return m_Method; }

    // PARAMS
    // 获取参数对应的VALUE
    std::string operator[]( const std::string & key ) const;
    // 添加params
    void addParameter( const std::string & key, const std::string & value );
    // 获取所有参数
    const Parameters & getParameters() const { return m_Parameters; }
    // 是否有这个参数
    bool hasParameter( const std::string & param ) const { return m_Parameters.end() != m_Parameters.find( param ); }

private:
    std::string m_URL;
    std::string m_URI;
    std::string m_Method;
    std::string m_ClientHost;
    Parameters m_Parameters;
};

// HTTP回应
class HttpResponse : public HttpMessage
{
public:
    static std::map<int32_t, std::string> Status;
    static std::map<int32_t, std::string> g_InitHttpStatus();
    HttpResponse();
    HttpResponse( const std::string & version );
    virtual ~HttpResponse();

public:
    int32_t getStatusCode() const { return m_StatusCode; }
    void setStatusCode( int32_t code ) { m_StatusCode = code; }
    const std::string & getReasonPhrase() const { return m_ReasonPhrase; }
    void setReasonPhrase( const char * reason ) { m_ReasonPhrase = reason; }
    const std::string & getHttpdVersion() const { return m_HttpdVersion; }
    void setHttpdVersion( const char * version ) { m_HttpdVersion = version; }

public:
    // 设置结果
    void setResult( int32_t code );
    // 序列化
    void serialize( std::string & buffer );

private:
    // 默认HEAD参数
    void addDefaultHeaders();

private:
    int32_t m_StatusCode;
    std::string m_HttpdVersion;
    std::string m_ReasonPhrase;
};

#endif
