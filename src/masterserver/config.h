
#ifndef __SRC_MASTERSERVER_CONFIG_H__
#define __SRC_MASTERSERVER_CONFIG_H__

#include <map>
#include <string>

#include "base/types.h"
#include "io/framework.h"
#include "utils/singleton.h"

namespace master
{

class AppConfigfile : public ISysConfigfile, public Singleton<AppConfigfile>
{
public :
    virtual bool load();
    virtual bool reload();
    virtual void unload();

public :
    size_t getDBCachesize() const { return m_Cachesize; }
    uint8_t getApiThreads() const { return m_ApiThreads; }
    const std::string & getApiURL() const { return m_ApiURL; }

    const Endpoint & getGMEndpoint() const { return m_GMEndpoint; }
    uint32_t getGMTimeoutseconds() const { return m_GMTimeoutseconds; }

    const Endpoint & getGlobalEndpoint() const { return m_GlobalEndpoint; }

private :
    friend class Singleton<AppConfigfile>;
    AppConfigfile();
    virtual ~AppConfigfile();

private :
    size_t          m_Cachesize;
    uint8_t         m_ApiThreads;
    std::string     m_ApiURL;

private :
    Endpoint        m_GMEndpoint;           // GM服务器
    uint32_t        m_GMTimeoutseconds;     // GM服务器超时时间

    Endpoint        m_GlobalEndpoint;       // 全局服务器
};

#define g_AppConfig AppConfigfile::instance()

}

#endif
