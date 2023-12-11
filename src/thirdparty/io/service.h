
#ifndef __SRC_IO_SERVICE_H__
#define __SRC_IO_SERVICE_H__

#include <cassert>
#include <functional>

#include "io.h"
#include "harbor.h"
#include "base/types.h"
#include "base/message.h"
#include "base/endpoint.h"

#include "utils/slice.h"
#include "utils/thread.h"

class ISysConfigfile;

struct SSMessage;
using ProcessCallback = std::function<void (HostID id, SSMessage * msg)>;

//
// 核心Harbor服务
//
class HarborService : public IIOService
{
public:
    HarborService(
        const char * module,
        ISysConfigfile * sysconf,
        utils::IWorkThread * thread );
    virtual ~HarborService();

    // 连接回调
    virtual bool onConnectFailed( int32_t result, const char * host, uint16_t port );
    virtual IIOSession * onConnectSucceed( sid_t sid, const char * host, uint16_t port );
    // 接受回调
    virtual IIOSession * onAccept( NetType type, uint16_t listenport, sid_t sid, const char * host, uint16_t port );

public:
    // 提交
    virtual bool post(
        void * iocontext,
        int32_t type, void * task )
    {
        (void)iocontext;
        return m_WorkThread->post( type, task );
    }

    // 提交模板
    template<class T>
        void POST( void * iocontext, int32_t type, T * task )
        {
            if ( !post( iocontext, type, task ) ) {
                delete task;
                task = nullptr;
            }
        }

    // 获取Harbor
    Harbor * harbor() { return &m_Harbor; }

    // 初始化
    // types    - 需要连接的后端服务器类型
    bool connects( const HostTypes & types = HostTypes() );

    // 断开所有连接
    void disconnects();

    // 重建
    // types    - 需要连接的后端服务器类型
    bool rebuild( const HostTypes & types = HostTypes() );

    // 发送数据
    int32_t send( sid_t sid, SSMessage * msg );
    int32_t sendByHostID( HostID id, SSMessage * msg );
    int32_t sendByRandom( HostType type, SSMessage * msg );

    // 广播
    int32_t broadcast( SSMessage * msg );
    int32_t broadcast( HostType type, SSMessage * msg );
    int32_t broadcast( const sids_t & tolist, SSMessage * msg );
    int32_t broadcast( const HostIDs & tolist, SSMessage * msg );

    // 广播框架图
    void broadcastFramework();
    // 处理默认消息
    void process( int32_t type, void * task );
    // 设置回调函数
    void setCallback( const ProcessCallback & callback ) { m_ProcessCallback = callback; }

private:
    friend class HarborSession;

    // 发送注册消息
    int32_t sendRegisterCommand( sid_t sid, const Endpoint & ep = Endpoint() );
    // 向管理服务器注册
    void register2( sid_t sid, HostType type, ZoneID zoneid, const std::string & procmark, const Endpoint & ep );

protected:
    Harbor m_Harbor;
    HostID m_HostID;
    HostType m_HostType;

protected:
    std::string m_Module;
    ISysConfigfile * m_SysConfig;
    utils::IWorkThread * m_WorkThread;
    ProcessCallback m_ProcessCallback;
};

#endif
