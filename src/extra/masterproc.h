
#ifndef __SRC_EXTRA_MASTERPROCESS_H__
#define __SRC_EXTRA_MASTERPROCESS_H__

#include <queue>
#include <stdint.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>

#include "base/types.h"
#include "base/endpoint.h"
#include "utils/lock.h"
#include "utils/thread.h"
#include "utils/singleton.h"
#include "domain/serverstats.h"

class ISysConfigfile;

//
// 初始化步骤
//
// 初始化未完成前，
// 本服务器对外不提供服务, 主线程不处理任何业务数据
// 当然从数据库返回的数据除外
// 也就是说，这边所有的步骤的数据来源只能是数据库返回的数据
//
enum
{
    eInitState_Variable = ( 1 << 0 ),   // Variable加载成功
    eInitState_Rank = ( 1 << 1 ),       // 排行榜加载成功
    eInitState_CharBrief = ( 1 << 2 ),  // 角色简要信息
    eInitState_Guild = ( 1 << 3 ),      // 公会详细信息
    eInitState_Blacklist = ( 1 << 4 ),  // 黑名单
    eInitState_WorldBoss = ( 1 << 5 ),  // 世界BOSS
    eInitState_Arena = ( 1 << 6 ),      // 竞技场
    // TODO: 更多的步骤
};

struct ReloadArgs
{
    uint32_t mode = 0;
    uint32_t transid = 0;

    ReloadArgs() = default;
    ReloadArgs( uint32_t mode_, uint32_t transid_ = 0 )
        : mode( mode_ ), transid( transid_ ) {}
    void clear() { mode = 0; transid = 0; }
};

//
// initialize()
//
// *** 服务器启动 ***
//
// flush()
// reload()
//
// *** 服务器停止 ***
//
// finalize()
//

class IApplication
{
public:
    IApplication( int32_t precision = 8, int32_t taskcount = 0 );
    virtual ~IApplication();
    // 初始化
    virtual bool initialize() = 0;
    // 启动
    virtual bool startup() = 0;
    // 刷新日志
    virtual void flush() = 0;
    // 重新加载
    virtual void reload() = 0;
    // 轮空
    virtual void idle() = 0;
    // 处理任务
    virtual void process( int32_t type, void * task ) = 0;
    // 停止
    virtual void stopend() = 0;
    // 销毁
    virtual void finalize() = 0;

public:
    utils::IWorkThread * appthread() { return m_Thread; }
    // 状态设置
    void update( uint32_t state, SwitchState st );
    uint32_t state() const { return m_InitState; }
    // 获取时间精度
    int32_t precision() const { return m_Precision; }
    int32_t taskcount() const { return m_MaxTaskCount; }
    // 热更
    void reloadone() { m_ReloadArgs.clear(); }
    const ReloadArgs & reloadargs() const { return m_ReloadArgs; }
    void reloadargs( const ReloadArgs & args ) { m_ReloadArgs = args; }

protected:
    friend class AppThread;
    utils::IWorkThread * m_Thread; // 逻辑线程
    int32_t m_Precision;           // 时间精度
    int32_t m_MaxTaskCount;        // 最大任务数
    ReloadArgs m_ReloadArgs;       // 重载参数
    uint32_t m_InitState;          // 初始化状态
};

//
// 主进程
//
class MasterProcess : public Singleton<MasterProcess>
{
public:
    // CPU时间
    double cputime() const;
    double cpuutilization() const;
    // 启动时间
    int64_t uptime() const;
    // 获取进程号
    pid_t pid() const { return getpid(); }
    // 磁盘用量
    int32_t diskusage() const;
    // 内存总量
    int64_t getUseMemory() const;
    // 获取时区
    int32_t timezone() const { return m_Timezone; }
    // 实例ID
    int32_t instid() const { return m_InstID; }
    // 服务器状态
    RunStatus status() const { return m_Status; }
    // 远端Master地址
    const Endpoint & endpoint() const { return m_Endpoint; }
    // 模块名
    const char * module() const { return m_Module.c_str(); }
    // 逻辑服务器
    IApplication * application() const { return m_Application; }

    //
    template<typename T> T env( const std::string & key, const T & default_value ) const {
        return default_value;
    }

    // 发送信号
    // 发送HUP的时候,可以携带参数
    void signal( int32_t signo, const ReloadArgs & args = ReloadArgs() );

    // 初始化
    void init( int argc, const char ** argv,
        const char * appname, const char * appversion );

    // 运行
    // server   - 服务器
    // sysconf  - 服务器配置
    int run( IApplication * server, ISysConfigfile * sysconf );

    // 释放脏页数据
    void purge();

    // 销毁
    int final();

private:
    friend class Singleton<MasterProcess>;
    MasterProcess();
    virtual ~MasterProcess();
    void help();
    void version();

private:
    int32_t m_InstID;
    int32_t m_Timezone;
    time_t m_StartTime;
    pthread_t m_MasterPid;
    std::string m_Appname;
    std::string m_AppVersion;
    std::string m_Module;
    RunStatus m_Status;
    Endpoint m_Endpoint;
    IApplication * m_Application;
};

#define MASTER MasterProcess::instance()
#define APPLICATION MasterProcess::instance().application()

#endif
