
#ifndef __SRC_IO_ACCESS_H__
#define __SRC_IO_ACCESS_H__

#include <string>

#include "base/types.h"
#include "base/message.h"
#include "base/endpoint.h"
#include "protocol/types.h"

#include "io/io.h"
#include "utils/crc.h"
#include "utils/slice.h"

//
// 接入会话
//
class AccessSession : public IIOSession
{
public:
    AccessSession();
    virtual ~AccessSession();
    virtual bool onValidate();
    virtual ssize_t onProcess( const char * buffer, size_t nbytes );
    virtual int32_t onTimeout();
    virtual int32_t onError( int32_t result );
    virtual void onMessage( MessageID cmd, const char * buffer, size_t length );

protected:
    // 提取/添加事务ID
    uint32_t takeTransID( MessageID cmd );
    void addTransID( MessageID cmd, uint32_t transid );
    // 设置token
    void setToken( const std::string & token ) { m_Token = token; }
    // 检查客户端的发包速率
    bool checkSendPacketRate();
    // 计算checksum
    inline uint8_t checksum( const char * buffer, size_t len );
    // 携带事务ID发送消息
    int32_t sendTransID( const char * buffer, size_t nbytes, uint32_t transid );

protected:
    std::string m_RecvBuff;
    int32_t m_RecvPackets;
    int64_t m_RecvTimestamp;
    std::string m_Token;
    utils::CRC16 m_Checksum;
    std::vector<std::pair<MessageID, uint32_t>> m_Transactions;
};

//
// 接入服务
//
class AccessService : public IIOService
{
public:
    AccessService(
        uint8_t nthreads, uint32_t nclients,
        int32_t nseconds, int32_t precision = 8, bool immediately = false );
    virtual ~AccessService();
    // 数据改造
    virtual char * onTransform( const char * buffer, size_t & nbytes );

public:
    // 超时时间
    int32_t getTimeoutSeconds() const { return m_TimeoutSeconds; }
    // 监听的端口号和IP地址
    const Endpoint & getBindEndpoint() const { return m_BindEndpoint; }
    void setBindEndpoint( const Endpoint & ep ) { m_BindEndpoint = ep; }
    // 发送
    int32_t send( sid_t id, MessageID cmd, const Slice & message );
    // 回应
    int32_t reply( sid_t id, MessageID cmd, const Slice & message );

private:
    friend class AccessSession;
    // 压缩
    static char * compress( CSHead & head, const char * buffer, size_t & nbytes );
    // 转换
    static char * transform( const char * buffer, size_t & nbytes, uint32_t transid = 0 );

private:
    Endpoint m_BindEndpoint;
    int32_t m_TimeoutSeconds;
};

#endif
