
#ifndef __SRC_HTTP_PARSER_H__
#define __SRC_HTTP_PARSER_H__

#include <string>
#include <stdint.h>

class HttpMessage;
class HttpRequest;
class HttpResponse;

class HttpParser
{
public:
    // 解析状态
    enum
    {
        eStartLine,
        eHeader,
        eContent,
        eCompleted
    };

public:
    HttpParser();
    ~HttpParser();

public:
    // 重置解释器
    void reset();
    // 是否解析完成
    bool isCompleted() const;
    // 获取消息(解释器中的消息被置空)
    HttpRequest * getRequest();
    HttpResponse * getResponse();
    HttpMessage * getMessage() const { return m_HttpMessage; }
    // 添加数据
    int32_t append( const char * buffer, uint32_t length );

private:
    // 解析StartLine
    uint32_t parseStartLine( const char * buffer, uint32_t length );
    // 解析HTTP包头
    uint32_t parseHeader( const char * buffer, uint32_t length );
    // 解析Chunked报文
    uint32_t parseChunked( const char * buffer, uint32_t length );
    // 解析HTTP报文
    uint32_t parseContent( const char * buffer, uint32_t length );

private:
    // 获取行数据
    // keeplineendings - 是否保留换行符
    static bool getline( const std::string & buffer,
        std::string & line, bool keeplineendings = true );
    // 处理POST
    static void processPostContent( HttpRequest * request );
    // 解析POST参数
    static void parsePostParams( HttpRequest * request, char * params );

private:
    int8_t m_Status;
    HttpMessage * m_HttpMessage; // HTTP消息
};

#endif
