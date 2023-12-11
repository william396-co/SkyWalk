
#ifndef __SRC_EXTRA_HTTPSERVICE_H__
#define __SRC_EXTRA_HTTPSERVICE_H__

#include "io/io.h"
#include "utils/types.h"
#include "utils/lrucache.h"

namespace utils {
class IWorkThread;
}

class HttpRequest;
class HttpService : public IIOService
{
public:
    HttpService( uint8_t nthreads, uint32_t nclients, uint32_t capacity = 100000 );
    virtual ~HttpService();
    // 接受新的连接
    virtual IIOSession * onAccept(
        NetType type, uint16_t listenport,
        sid_t id, const char * host, uint16_t port );

public:
    void setAppthread( utils::IWorkThread * t ) { m_AppThread = t; }
    void setKeepaliveSeconds( int32_t seconds ) { m_KeepaliveSeconds = seconds; }

    // 检查是否受限制
    bool isRestricted( const std::string & account, int32_t count = 80 );

    // 发送错误码和内容(基于JSON)
    bool sendErrorcode( sid_t sid,
        HttpRequest * request,
        const std::string & server, int32_t result );
    bool sendErrorcode( sid_t sid,
        const std::string & version,
        const std::string & connection,
        const std::string & server, int32_t result );
    bool sendContent( sid_t sid,
        HttpRequest * request,
        const std::string & server, const std::string & content,
        const std::string & contenttype = "application/json; charset=UTF-8" );
    bool sendContent( sid_t sid,
        const std::string & version,
        const std::string & connection, const std::string & server,
        const std::string & content, const std::string & contenttype = "application/json; charset=UTF-8" );

private:
    typedef utils::LRUCache<std::string, std::pair<int32_t, int64_t>> AccessFilter;

    utils::IWorkThread * m_AppThread; //
    int32_t m_KeepaliveSeconds;       // 超时时间
    AccessFilter m_AccessFilter;      // 访问过滤器
};

#endif
