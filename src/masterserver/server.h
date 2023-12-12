
#ifndef __SRC_MASTERSERVER_SERVER_H__
#define __SRC_MASTERSERVER_SERVER_H__

#include <map>
#include <deque>
#include <pthread.h>

#include "io/io.h"
#include "io/service.h"
#include "base/base.h"
#include "base/message.h"
#include "base/endpoint.h"
#include "utils/thread.h"
#include "utils/singleton.h"
#include "extra/masterproc.h"

class TickService;
class HttpService;
class CurlThreads;

namespace utils {
class TransactionManager;
}

namespace master {

class GameApp : public IApplication, public Singleton<GameApp>
{
private:
    friend class Singleton<GameApp>;

    GameApp() = default;
    virtual ~GameApp() = default;
    virtual bool initialize() { return true; }
    virtual bool startup();
    virtual void alter( uint32_t state );
    virtual void flush();
    virtual void reload();
    virtual void idle();
    virtual void process( int32_t type, void * task );
    virtual void stopend();
    virtual void finalize() {}

public:
    bool isComposeMail();

    uint32_t getReloadTransid() const { return m_ReloadTransid; }
    void setReloadTransid( uint32_t id ) { m_ReloadTransid = id; }

    HttpService * getHttpService() { return m_HttpService; }
    HarborService * getHarborService() const { return m_HarborService; }
    CurlThreads * getCurlThreads() { return m_CurlThreads; }
    utils::TransactionManager * getTransactionManager() { return m_TransactionManager; }

private:
    bool startHarborService();
    void stopHarborService();
    bool startHttpService();
    void stopHttpService();

private:
    CurlThreads * m_CurlThreads = nullptr;     // curl线程
    TickService * m_TickService = nullptr;     //
    HttpService * m_HttpService = nullptr;     // httpserver
    HarborService * m_HarborService = nullptr; // 网络服务

    time_t m_Timestamp = 0;
    uint32_t m_ComposeMailCount = 0;
    uint32_t m_ReloadTransid = 0; // 重新加载事务ID
    utils::TransactionManager * m_TransactionManager = nullptr;
};

#define g_HarborService GameApp::instance().getHarborService()
#define g_HttpAgent GameApp::instance().getCurlThreads()->rotate()
#define g_HttpService GameApp::instance().getHttpService()
#define g_TransactionManager GameApp::instance().getTransactionManager()

} // namespace master

#endif
