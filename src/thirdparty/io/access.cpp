
#include "base/base.h"
#include "base/limits.h"

#include "utils/lz4.h"
#include "utils/types.h"
#include "utils/xtime.h"
#include "utils/streambuf.h"

#include "message/gateprotocol.h"

#include "task.h"
#include "access.h"

AccessSession::AccessSession()
    : m_RecvPackets( 0 ),
      m_RecvTimestamp( 0 )
{
    // m_RecvBuff.reserve( 65536 * 2 );
    // NOTICE: 兼顾内存, 这边不做内存的预设
    // NOTICE: 有个前提，客户端发出的包不会太大
}

AccessSession::~AccessSession()
{}

bool AccessSession::onValidate()
{
    return true;
}

ssize_t AccessSession::onProcess( const char * buffer, size_t nbytes )
{
    ssize_t nprocess = 0;

    if ( unlikely( !onValidate() ) ) {
        LOG_ERROR( "AccessSession::onProcess(Sid:{}, {}) : this Connection is INVALIDATE .\n", id(), endpoint() );
        return -1;
    }

    for ( ;; ) {
        CSHead head;
        size_t nleft = nbytes - nprocess;
        const char * buf = buffer + nprocess;

        // 数据包太小
        if ( nleft < sizeof( CSHead ) ) {
            break;
        }

        // [CSHead][......]
        StreamBuf unpack( buf, nleft );
        unpack.decode( head.cmd );
        unpack.decode( head.flags );
        unpack.decode( head.size );

        // TODO: 校验包中的数据长度是否合法

        // 合法的数据包
        if ( nleft < head.size + sizeof( CSHead ) ) {
            break;
        }

        // 计算校验码
        // TODO: 需要考虑分片包的校验码
        // TODO: 总包一个校验码还是每个分片的包都一个
        uint8_t chksum = checksum( buf + sizeof( CSHead ), head.size );
        if ( chksum != (uint8_t)-1
            && chksum != ( uint8_t )( head.flags >> 8 ) ) {
            LOG_ERROR( "AccessSession::onProcess(Sid:{}, {}) : this MessageID(CMD:{:#06x}) has a bad CHECKSUM({}, {}, {}) .\n",
                id(), endpoint(), head.cmd, chksum, ( uint8_t )( head.flags >> 8 ), head.flags );
            return -2;
        }

        // 检查发包速率
        if ( !checkSendPacketRate() ) {
            LOG_ERROR( "AccessSession::onProcess(Sid:{}, {}) : this Client Send-Packets-Rate is too FAST .\n", id(), endpoint() );
            return -3;
        }

        // 解析flags字段
        if ( unlikely( head.flags & CSHead::MORE ) ) {
            m_RecvBuff += std::string( buf + sizeof( CSHead ), head.size );
        } else {
            // 完整的逻辑包
            if ( likely( m_RecvBuff.empty() ) ) {
                // 没有分包的数据包,
                // 不用拷贝到m_RecvBuff中, 直接提交
                onMessage( head.cmd,
                    (const char *)( buf + sizeof( CSHead ) ), head.size );
            } else {
                m_RecvBuff += std::string( buf + sizeof( CSHead ), head.size );
                // 提交数据包
                onMessage( head.cmd, m_RecvBuff.data(), m_RecvBuff.size() );

                // 接收到完整的逻辑包后, 需要清空接收缓冲区
                m_RecvBuff.clear();
            }
        }

        nprocess += ( head.size + sizeof( CSHead ) );
    }

    return nprocess;
}

int32_t AccessSession::onTimeout()
{
    LOG_TRACE( "AccessSession::onTimeout(Sid:{}, {}) .\n", id(), endpoint() );
    return -1;
}

int32_t AccessSession::onError( int32_t result )
{
    LOG_TRACE( "AccessSession::onError(Sid:{}, {}): {:#010x} .\n", id(), endpoint(), result );
    return -1;
}

void AccessSession::onMessage( MessageID cmd, const char * buffer, size_t length )
{}

uint32_t AccessSession::takeTransID( MessageID cmd )
{
    uint32_t result = 0;

    auto pos = std::find_if(
        m_Transactions.begin(), m_Transactions.end(),
        [cmd]( auto & kv ) -> bool { return kv.first == cmd; } );
    if ( pos != m_Transactions.end() ) {
        result = pos->second;
        m_Transactions.erase( pos );
    }

    return result;
}

void AccessSession::addTransID( MessageID cmd, uint32_t transid )
{
    m_Transactions.emplace_back( cmd, transid );
}

int32_t AccessSession::sendTransID( const char * buffer, size_t nbytes, uint32_t transid )
{
    char * buf = AccessService::transform( buffer, nbytes, transid );
    if ( buf == nullptr ) {
        return -1;
    }

    return send( buf, nbytes, true );
}

bool AccessSession::checkSendPacketRate()
{
    if ( m_RecvPackets == 0 ) {
        m_RecvTimestamp = utils::now();
    }

    ++m_RecvPackets;

    if ( m_RecvPackets > eMaxRecvPacketsRate ) {
        int64_t last = m_RecvTimestamp;

        m_RecvPackets = 0;
        m_RecvTimestamp = utils::now();

        return m_RecvTimestamp - last > 1000;
    }

    return true;
}

uint8_t AccessSession::checksum( const char * buffer, size_t len )
{
    if ( !m_Token.empty() ) {
        uint16_t mask1 = 8520, mask2 = 1087;
        uint16_t chksum = m_Checksum.calc( buffer, len );

        // 计算校验和掩码
        if ( m_Token.size() >= 3 ) {
            mask2 = m_Token[2];
            mask1 = (m_Token[0] << 8) | m_Token[1];
        }

        chksum %= mask1;
        chksum *= mask2;
        return ( uint8_t )( chksum & 0x00ff );
    }

    return -1;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

AccessService::AccessService( uint8_t nthreads, uint32_t nclients, int32_t nseconds, int32_t precision, bool immediately )
    : IIOService( nthreads, nclients, precision, immediately ),
      m_TimeoutSeconds( nseconds )
{}

AccessService::~AccessService()
{}

char * AccessService::onTransform( const char * buffer, size_t & nbytes )
{
    return AccessService::transform( buffer, nbytes );
}

int32_t AccessService::send( sid_t sid, MessageID cmd, const Slice & message )
{
    uint16_t flags = 0;
    uint16_t bodylen = message.size();

    StreamBuf packet( message.size() + sizeof( CSHead ) );
    // 包头
    packet.encode( cmd );
    packet.encode( flags );
    packet.encode( bodylen );
    // 包体
    packet.append( (const char *)message.data(), message.size() );

    Slice buf = packet.slice();
    return IIOService::send( sid, buf.data(), buf.size(), true );
}

int32_t AccessService::reply( sid_t sid, MessageID cmd, const Slice & message )
{
    uint16_t flags = 0;
    uint16_t bodylen = message.size();

    StreamBuf packet( message.size() + sizeof( CSHead ) );
    // 包头
    packet.encode( cmd );
    packet.encode( flags );
    packet.encode( bodylen );
    // 包体
    packet.append( (const char *)message.data(), message.size() );

    return perform( sid, eTaskType_Response, new ResponseTask( cmd, packet.slice() ), ResponseTask::recycle );
}

char * AccessService::compress( CSHead & head, const char * buffer, size_t & nbytes )
{
    // 输出的长度
    size_t outlen = utils::LZ4::getCompressBound( nbytes - sizeof( CSHead ) );
    char * output = (char *)std::malloc( outlen + sizeof( CSHead ) );
    if ( output == nullptr ) {
        return (char *)buffer;
    }

    // 压缩失败
    if ( !utils::LZ4::compress(
             std::string( buffer, nbytes ).substr( sizeof( CSHead ) ),
             (void *)( output + sizeof( CSHead ) ), outlen ) ) {
        return (char *)buffer;
    }

    // 返回head+body
    nbytes = outlen;
    nbytes += sizeof( CSHead );
    // 修改包头
    head.size = outlen;
    head.flags |= CSHead::ZIP;

    return output;
}

char * AccessService::transform( const char * buffer, size_t & nbytes, uint32_t transid )
{
    uint16_t threshold = USHRT_MAX;
    char * output = const_cast<char *>( buffer );

    //
    // 数据包不需要改造的情况
    //
    // 1. 不需要压缩
    // 2. 不需要分片
    // 3. 不需要回写事务
    // 4. 不需要加密
    //
    if ( nbytes <= threshold && transid == 0 ) {
        return output;
    }

    CSHead head;

    // 解包头
    StreamBuf unpack( buffer, nbytes );
    unpack.decode( head.cmd );
    unpack.decode( head.flags );
    unpack.decode( head.size );

    // 强制置空状态位
    head.flags = 0x0000;

    if ( transid != 0 ) {
        // TODO: 写入transid
    }

    // TODO: 压缩
    // TODO: 加密(加密是支持就地加密的，不需要进行内存拷贝)

    if ( nbytes <= threshold ) {
        // 就地改造
        StreamBuf pack( output, nbytes );
        pack.encode( head.cmd );
        pack.encode( head.flags );
        pack.encode( head.size );
        // TODO: 写入transid

        // 清空消息
        pack.clear();
    } else {
        StreamBuf pack;
        size_t offset = 0;

        // 计算BODY的长度
        buffer += sizeof( CSHead );
        nbytes -= sizeof( CSHead );

        // 数据包分片
        while ( offset < nbytes ) {
            uint16_t len = ( nbytes - offset ) < threshold ? ( nbytes - offset ) : threshold;
            uint16_t flags = offset + len == nbytes ? CSHead::END : CSHead::MORE;

            // 序列化HEAD
            pack.encode( head.cmd );
            pack.encode( flags |= head.flags );
            pack.encode( len );
            // TODO: 写入transid

            // 序列化BODY
            pack.append( buffer + offset, len );

            // 偏移量
            offset += len;
        }

        // 保存数据
        nbytes = pack.size();
        output = pack.data();

        // 清空消息
        pack.clear();
    }

    return output;
}
