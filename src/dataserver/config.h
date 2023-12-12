
#ifndef __SRC_DATASERVER_CONFIG_H__
#define __SRC_DATASERVER_CONFIG_H__

#include "base/types.h"
#include "base/endpoint.h"

#include "utils/file.h"
#include "utils/singleton.h"
#include "io/framework.h"

namespace data {

class AppConfigfile final : public ISysConfigfile, public Singleton<AppConfigfile>
{
public:
    virtual bool load() final;
    virtual bool reload() final;
    virtual void unload() final;

public:
    // DB线程个数
    uint8_t getDBThreadCount() const { return m_DBThreadCount; }
    // 刷新时间
    uint32_t getFlushInterval() const { return m_FlushInterval; }
    // 最大缓存时间
    uint32_t getCacheLifetime() const { return m_CacheLifetime; }
    // 获取启动脚本
    std::string getRootPath() const;
    const std::string & getStartupScript() const { return m_StartupScript; }
    const std::string & getRoutineScript() const { return m_RoutineScript; }

public:
    // Database
    const std::string & getDBHost() const { return m_DBHost; }
    uint16_t getDBPort() const { return m_DBPort; }
    const std::string & getDatabase() const { return m_Database; }
    const std::string & getDBUsername() const { return m_DBUsername; }
    const std::string & getDBPassword() const { return m_DBPassword; }
    const std::string & getDBCharsets() const { return m_DBCharsets; }
    bool isIgnoreErrorcode( int32_t code ) const;

private:
    friend class Singleton<AppConfigfile>;
    AppConfigfile();
    virtual ~AppConfigfile() final;

private:
    uint8_t m_DBThreadCount;
    uint32_t m_FlushInterval;
    uint32_t m_CacheLifetime;
    std::string m_StartupScript;
    std::string m_RoutineScript;

    IntVec m_IgnoreErrors; // 忽略的错误码

private:
    std::string m_DBHost;     // 数据库地址
    uint16_t m_DBPort;        // 数据库端口
    std::string m_DBUsername; // 数据库用户名
    std::string m_DBPassword; // 数据库密码
    std::string m_DBCharsets; // 连接编码
    std::string m_Database;   // 访问的库名
};

#define g_AppConfig AppConfigfile::instance()

} // namespace data

#endif
