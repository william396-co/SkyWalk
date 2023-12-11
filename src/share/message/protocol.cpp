
#include <cstdlib>

#include "protocol.h"

SSMessage::SSMessage()
    : sid( 0 ),
      space( nullptr ),
      length( 0 )
{
    head.cmd = 0;
    head.flags = 0;
    head.size = 0;
    head.type = eMessageType_Invalid;
}

SSMessage::SSMessage( MessageID cmd )
    : sid( 0 ),
      space( nullptr ),
      length( 0 )
{
    head.flags = 0;
    head.size = 0;
    head.cmd = cmd;
    head.type = eMessageType_Command;
}

SSMessage::SSMessage( MessageType type, MessageID cmd )
    : sid( 0 ),
      space( nullptr ),
      length( 0 )
{
    head.flags = 0;
    head.size = 0;
    head.cmd = cmd;
    head.type = type;
}

SSMessage::~SSMessage()
{
    sid = 0;
    length = 0;

    if ( space != nullptr ) {
        std::free( space );
        space = nullptr;
    }
}

void SSMessage::encode_header( StreamBuf & pack )
{
    // 计算长度
    head.size = pack.size();

    // HEAD
    pack.reset();
    pack.encode( head.type );
    pack.encode( head.flags );
    pack.encode( head.cmd );
    pack.encode( head.size );
}

bool SSMessage::make( const char * buffer, size_t len )
{
    if ( space != nullptr ) {
        return false;
    }

    space = (char *)std::malloc( len );
    if ( space != nullptr ) {
        length = len;
        std::memcpy( (void *)space, (void *)buffer, len );
    }

    return space != nullptr;
}

////////////////////////////////////////////////////////////////////////////////

Slice TransferMessage::encode()
{
    StreamBuf pack( 4096, sizeof( SSHead ) );

    // BODY
    pack.encode( roleid );
    pack.encode( oob );
    pack.append( msgbody.data(), msgbody.size() );
    // 计算长度
    encode_header( pack );

    return pack.slice();
}

bool TransferMessage::decode( const Slice & data )
{
    // 拷贝数据
    make( data.data(), data.size() );
    return fastdecode( static_cast<const char *>( space ), length );
}

bool TransferMessage::fastdecode( const char * data, uint32_t length )
{
    // 解析数据
    StreamBuf unpack( data, length );
    unpack.decode( roleid );
    unpack.decode( oob );
    msgbody = Slice(
        unpack.data() + unpack.size(),
        unpack.length() - unpack.size() );

    return true;
}

Slice TraverseMessage::encode()
{
    StreamBuf pack( 4096, sizeof( SSHead ) );

    // BODY
    pack.encode( fromsid );
    pack.encode( oob );
    pack.append( msgbody.data(), msgbody.size() );
    // 计算长度
    encode_header( pack );

    return pack.slice();
}

bool TraverseMessage::decode( const Slice & data )
{
    // 拷贝数据
    make( data.data(), data.size() );
    return fastdecode( static_cast<const char *>( space ), length );
}

bool TraverseMessage::fastdecode( const char * data, uint32_t length )
{
    StreamBuf unpack( data, length );
    unpack.decode( fromsid );
    unpack.decode( oob );
    msgbody = Slice(
        unpack.data() + unpack.size(),
        unpack.length() - unpack.size() );

    return true;
}

Slice ResponseMessage::encode()
{
    StreamBuf pack( 4096, sizeof( SSHead ) );

    // BODY
    pack.encode( roleid );
    pack.append( msgbody.data(), msgbody.size() );
    // 计算长度
    encode_header( pack );

    return pack.slice();
}

bool ResponseMessage::decode( const Slice & data )
{
    // 拷贝数据
    make( data.data(), data.size() );
    return fastdecode( static_cast<const char *>( space ), length );
}

bool ResponseMessage::fastdecode( const char * data, uint32_t length )
{
    // 解析数据
    StreamBuf unpack( data, length );
    unpack.decode( roleid );
    msgbody = Slice(
        unpack.data() + unpack.size(),
        unpack.length() - unpack.size() );

    return true;
}

Slice BroadcastMessage::encode()
{
    StreamBuf pack( 4096, sizeof( SSHead ) );

    // BODY
    pack.encode( channel );
    pack.encode( channelid );
    if ( channel == eChannel_RoleList ) {
        pack.encode( rolelist );
    }
    pack.encode( exceptid );
    pack.append( msgbody.data(), msgbody.size() );
    // 计算长度
    encode_header( pack );

    return pack.slice();
}

bool BroadcastMessage::decode( const Slice & data )
{
    // 拷贝数据
    make( data.data(), data.size() );

    // 解析数据
    StreamBuf unpack(
        const_cast<const char *>( space ), length );
    unpack.decode( channel );
    unpack.decode( channelid );
    if ( channel == eChannel_RoleList ) {
        unpack.decode( rolelist );
    }
    unpack.decode( exceptid );
    msgbody = Slice(
        unpack.data() + unpack.size(),
        unpack.length() - unpack.size() );

    return true;
}

bool BroadcastMessage::fastdecode( const char * data, uint32_t length )
{
    // 解析数据
    StreamBuf unpack( data, length );
    unpack.decode( channel );
    unpack.decode( channelid );
    if ( channel == eChannel_RoleList ) {
        unpack.decode( rolelist );
    }
    unpack.decode( exceptid );
    msgbody = Slice(
        unpack.data() + unpack.size(),
        unpack.length() - unpack.size() );

    return true;
}

Slice PingCommand::encode()
{
    StreamBuf pack( 64, sizeof( SSHead ) );

    // 计算长度
    encode_header( pack );

    return pack.slice();
}

bool PingCommand::decode( const Slice & data )
{
    // 解析数据
    StreamBuf unpack(
        data.data(), data.size() );

    return true;
}
