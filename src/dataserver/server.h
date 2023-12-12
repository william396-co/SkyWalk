#pragma once

#include <unordered_map>

#include "utils/types.h"
#include "utils/thread.h"
#include "utils/singleton.h"

#include "base/types.h"
#include "base/database.h"
#include "scheme/sqlbind.h"
#include "extra/masterproc.h"

class TickService;
class HarborService;

namespace utils {
class TransactionManager;
}

namespace data {

//
class BinLogger;
class DBThreads;
class TableCache;

// 代理服务器
class GameApp final : public IApplication, public Singleton<GameApp>
{
private:
    friend class Singleton<GameApp>;
    GameApp() : IApplication(4) {};
    virtual ~GameApp() final = default;
    virtual bool initialize() final;
    virtual bool startup() final;
    virtual void flush() final {}
    virtual void reload() final {}
    virtual void idle() final;
    virtual void process( int32_t type, void * task ) final;
    virtual void stopend() final;
    virtual void finalize() final {}

public:
    // 数据原型
    TableProtoType * getPrototype() { return &m_ProtoType; }
    // 获取缓存池
    TableCache * getCache( const Tablename & table ) const;

    // BinLogger 和 DB线程池
    BinLogger * getBinLogger() const { return m_BinLogger; }
    DBThreads * getDBThreads() const { return m_DBThreads; }

    // 其他组件(网络服务，定时器，事务管理)
    TickService * getTickService() const { return m_TickService; }
    HarborService * getHarborService() const { return m_HarborService; }
    utils::TransactionManager * getTransManager() const { return m_TransManager; }

private:
    using DatabaseCache = std::unordered_map<Tablename, TableCache *>;

    // 创建缓存池
    void initCaches();
    void finalCaches();

    bool recover();
    bool runStartupScript();

    // 打开/关闭Harbor
    bool startHarborService();
    void stopHarborService();

private:
    BinLogger * m_BinLogger = nullptr;                    // 数据日志
    TableProtoType m_ProtoType;                           // 数据原型
    DatabaseCache m_DatabaseCache;                        // 数据库缓存
    DBThreads * m_DBThreads = nullptr;                    // 数据库线程组
    TickService * m_TickService = nullptr;                // 定时器
    HarborService * m_HarborService = nullptr;            // 网络服务
    utils::TransactionManager * m_TransManager = nullptr; // 事务管理器
};

#define g_TransManager GameApp::instance().getTransManager()
#define g_TickService GameApp::instance().getTickService()
#define g_DataPrototype GameApp::instance().getPrototype()
#define g_HarborService GameApp::instance().getHarborService()

template<class T>
inline bool POST( int32_t type, T * task )
{
    if ( !GameApp::instance().appthread()->post( type, task ) ) {
        delete task;
        task = nullptr;
        return false;
    }

    return true;
}

} // namespace data

