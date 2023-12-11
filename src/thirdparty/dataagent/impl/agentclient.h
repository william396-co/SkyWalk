
#ifndef __SRC_DATAAGENT_IMPL_AGENTCLIENT_H__
#define __SRC_DATAAGENT_IMPL_AGENTCLIENT_H__

#include <pthread.h>

#include "io/io.h"
#include "message/dbprotocol.h"

namespace data {

class AgentClient;
class DataAgentImpl;

class AgentSession final : public IIOSession
{
public:
    AgentSession( AgentClient * c );
    virtual ~AgentSession() final;
    virtual int32_t onStart() final;
    virtual ssize_t onProcess( const char * buffer, size_t nbytes ) final;
    virtual int32_t onTimeout() final;
    virtual int32_t onKeepalive() final;
    virtual int32_t onError( int32_t result ) final;
    virtual void onShutdown( int32_t way ) final;

private:
    AgentClient * m_AgentClient;
};

class AgentClient final : public IIOService
{
public:
    AgentClient( DataAgentImpl * agent );
    virtual ~AgentClient() final;
    virtual bool onConnectFailed( int32_t result, const char * host, uint16_t port ) final;
    virtual IIOSession * onConnectSucceed( sid_t id, const char * host, uint16_t port ) final;

public:
    // 超时时间
    uint32_t timeout() const;
    // 提交消息
    void post( int32_t type, void * task );
    // 发送
    bool send( sid_t sid, SSMessage * message );
    //
    DataAgentImpl * getAgent() { return m_DataAgent; }

private:
    DataAgentImpl * m_DataAgent; // 数据代理
};

} // namespace data

#endif
