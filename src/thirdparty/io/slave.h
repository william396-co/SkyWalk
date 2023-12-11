
#ifndef __SRC_IO_SLAVE_H__
#define __SRC_IO_SLAVE_H__

#include <mutex>
#include <condition_variable>

#include "io.h"
#include "task.h"
#include "utils/singleton.h"

struct SSMessage;
class ISysConfigfile;

namespace utils {
class IWorkThread;
}

class Slave final : public IIOService, public Singleton<Slave>
{
public:
    // 初始化
    bool initialize(
        const char * module,
        HostType type, const Endpoint & ep,
        utils::IWorkThread * thread, ISysConfigfile * sysconf, int32_t timeout = 30 );
    // 销毁
    void finalize();

    // 获取主机ID
    HostID hostid();
    HostType hosttype();

    // 发送数据
    int32_t send( SSMessage * msg );

    // 提交
    bool post( int32_t type, void * task );

private:
    friend class SlaveConnection;
    friend class Singleton<Slave>;
    Slave();
    virtual ~Slave() final;
    virtual bool onConnectFailed(
        int32_t result, const char * host, uint16_t port ) final;
    virtual IIOSession * onConnectSucceed(
        sid_t sid, const char * host, uint16_t port ) final;

    // 更新框架
    void updateFramework( SSMessage * message );

    // 获取主机ID
    HostID getHostID( HostType type ) const;

    // 发送注册消息
    void sendRegisterCommand( HostID id, HostType type, const Endpoint & ep = Endpoint() );

private:
    std::string m_Module;
    std::mutex m_Lock;
    std::condition_variable m_Cond;

private:
    HostID m_HostID;
    HostType m_HostType;
    sid_t m_ClientSid;
    ISysConfigfile * m_SysConf;
    utils::IWorkThread * m_WorkThread;
};

#endif
