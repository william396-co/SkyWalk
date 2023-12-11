#pragma once
#include <unordered_map>
#include <functional>

#include "base/types.h"
#include "base/message.h"
#include "base/endpoint.h"

#include "io/io.h"
#include "utils/slice.h"
#include "utils/types.h"
#include "utils/singleton.h"
#include "utils/streambuf.h"
#include "utils/reflection.h"

////////////////////////////////////////////////////////////////////////////////

// 通用命令
enum
{
    eSSCommand_Invalid = 0x0000, // 非法消息

    eSSCommand_Ping = 0x0001,          // PING包
    eSSCommand_Register = 0x0002,      // 服务器注册
    eSSCommand_RegSucceed = 0x0003,    // 服务器注册结果
    eSSCommand_Unregister = 0x0004,    // 服务器反注册
    eSSCommand_Ntpdate = 0x0005,       // 同步时间
    eSSCommand_Framework = 0x0006,     // 服务器框架信息
    eSSCommand_Reload = 0x0007,        // 重新加载指令
    eSSCommand_ShellCommand = 0x0008,  // 系统命令
    eSSCommand_RoleStatus = 0x0009,    // 同步角色状态
    eSSCommand_ServerStatus = 0x000A,  // 查询服务器状态
    eSSCommand_DebugAccount = 0x000B,  // 调试账号
    eSSCommand_Skipday = 0x000C,       // 隔天信号
    eSSCommand_Save2Document = 0x000D, // 保存文件
};

// 定义频道
enum
{
    eChannel_Invalid = 0,  // 非法的频道
    eChannel_World = 1,    // 世界频道
    eChannel_Scene = 2,    // 场景频道
    eChannel_Team = 3,     // 组队频道
    eChannel_Guild = 4,    // 公会频道
    eChannel_RoleList = 5, // 指定角色ID列表
};

// 消息基类
struct SSMessage
{
public:
    SSMessage();
    SSMessage( MessageID cmd );
    SSMessage( MessageType type, MessageID cmd );
    virtual ~SSMessage();

    // 克隆
    virtual SSMessage * clone( bool isfull ) = 0;
    // 序列化消息
    virtual Slice encode() = 0;
    // 反序列化包体
    virtual bool decode( const Slice & data ) = 0;

    // packhead
    void encode_header( StreamBuf & pack );

    // 扩展space
    bool make( const char * buffer, size_t len );
    Slice spaceslice() { return Slice( space, length ); }

public:
    sid_t sid;     // 会话ID
    SSHead head;   // 消息头
    char * space;  // 缓存
    size_t length; // 缓存的长度
};

// 通用解析器
class GeneralDecoder : public Singleton<GeneralDecoder>
{
public:
    uint16_t get( MessageID cmd ) const;
    SSMessage * decode( sid_t sid,
        const SSHead & head, const Slice & body );
    void add( MessageID cmd,
        const std::function<SSMessage *()> & func, uint16_t mask = 0 );

private:
    friend class Singleton<GeneralDecoder>;
    GeneralDecoder();
    ~GeneralDecoder();

    std::unordered_map<MessageID, uint16_t> m_MessageMap; // 主线程
    std::unordered_map<MessageID, std::function<SSMessage *()>> m_MessagePrototype; // IO线程
};

// 编解码助手
template<typename T>
struct Codec
{
    T * copy( bool isfull ) {
        T * data = new T;
        T * self = static_cast<T *>( this );
        if ( isfull && data != nullptr ) {
            if ( self->space == nullptr || self->length == 0 ) {
                *data = *self;
            } else {
                //
                data->sid = self->sid;
                data->head = self->head;
                // 指定space或者length的情况下, 需要解码
                data->unserialize( self->spaceslice(), true );
            }
        }
        return data;
    }

    Slice serialize( size_t len = 1024 ) {
        T * self = static_cast<T *>( this );
        StreamBuf pack( len, sizeof(SSHead) );
        utils::reflect::for_each( *self, [&pack]( auto & v, size_t i ) {
            pack.encode( v );
        } );
        self->encode_header( pack );
        return pack.slice();
    }

    bool unserialize( const Slice & data, bool deep = false ) {
        Slice src = data;
        T * self = static_cast<T *>( this );
        if ( deep ) {
            self->make( data.data(), data.size() );
            src = self->spaceslice();
        }
        StreamBuf unpack( src.data(), src.size() );
        utils::reflect::for_each( *self, [&unpack]( auto & v, size_t i ) {
            unpack.decode( v );
        } );
        return true;
    }
};

// 消息开始
#define MESSAGE_BEGIN( MESSAGE_NAME, ID, LEN )                                  \
struct MESSAGE_NAME : SSMessage, Codec<MESSAGE_NAME>                            \
{                                                                               \
public :                                                                        \
    MESSAGE_NAME()                                                              \
        : SSMessage( ID ) {}                                                    \
    virtual ~MESSAGE_NAME() {}                                                  \
                                                                                \
    virtual SSMessage * clone( bool isfull ) { return copy( isfull ); }         \
    virtual Slice encode() { return serialize( LEN ); }                         \
    virtual bool decode( const Slice & data ) { return unserialize( data ); }   \

// 消息结束
#define MESSAGE_END( MESSAGE_NAME, ... )                                        \
};                                                                              \
REFLECTION( MESSAGE_NAME, __VA_ARGS__ );                                        \

////////////////////////////////////////////////////////////////////////////////

// 转发消息
struct TransferMessage : SSMessage
{
public:
    TransferMessage() {}
    virtual ~TransferMessage() {}

    virtual SSMessage * clone( bool isfull ) { return nullptr; }
    virtual Slice encode();
    virtual bool decode( const Slice & data );

    bool fastdecode( const char * data, uint32_t length );

public:
    UnitID roleid = 0;  // 角色ID
    Slice oob;          // 带外数据
    Slice msgbody;      // 消息内容
};

// 穿透消息
struct TraverseMessage : SSMessage
{
public:
    TraverseMessage() {}
    virtual ~TraverseMessage() {}

    virtual SSMessage * clone( bool isfull ) { return nullptr; }
    virtual Slice encode();
    virtual bool decode( const Slice & data );

    bool fastdecode( const char * data, uint32_t length );

public:
    sid_t fromsid = 0;  // 客户端会话ID
    Slice oob;          // 带外数据
    Slice msgbody;      // 消息内容
};

// 回应消息
struct ResponseMessage : SSMessage
{
public:
    ResponseMessage() {}
    virtual ~ResponseMessage() {}

    virtual SSMessage * clone( bool isfull ) { return nullptr; }
    virtual Slice encode();
    virtual bool decode( const Slice & data );

    bool fastdecode( const char * data, uint32_t length );

public:
    UnitID roleid = 0;  // 角色ID
    Slice msgbody;      // 消息内容
};

// 广播消息
struct BroadcastMessage : SSMessage
{
public:
    BroadcastMessage() {}
    virtual ~BroadcastMessage() {}

    virtual SSMessage * clone( bool isfull ) { return nullptr; }
    virtual Slice encode();
    virtual bool decode( const Slice & data );

    bool fastdecode( const char * data, uint32_t length );

public:
    uint8_t channel = 0;        // 频道类型
    ChannelID channelid = 0;    // 频道ID
    UnitIDList rolelist;        // 角色列表
    UnitID exceptid = 0;        // 不包括的角色ID
    Slice msgbody;              // 消息内容
};

////////////////////////////////////////////////////////////////////////////////

// PING包
struct PingCommand : SSMessage
{
public:
    PingCommand()
        : SSMessage( eSSCommand_Ping ) {}
    virtual ~PingCommand() {}

    virtual SSMessage * clone( bool isfull ) { return new PingCommand; }
    virtual Slice encode();
    virtual bool decode( const Slice & data );
};

// 注册消息
MESSAGE_BEGIN( RegisterCommand, eSSCommand_Register, 1024 )
    HostID hostid = 0;                  // 主机ID
    ZoneID zoneid = 0 ;                 // 区服id
    HostType hosttype = HostType::None; // 主机类型
    std::string procmark;               // 进程标识
    Endpoint endpoint;                  // 可选项
    RegisterCommand( sid_t id )
        : SSMessage( eSSCommand_Register ) { sid = id; }
MESSAGE_END( RegisterCommand, hostid, zoneid, hosttype, procmark, endpoint );

// 注册成功消息
MESSAGE_BEGIN( RegSucceedCommand, eSSCommand_RegSucceed, 1024 )
    HostID hostid = 0;                  // 主机ID
    ZoneID zoneid = 0;                  // 区服id
    HostType hosttype = HostType::None; // 主机类型
    Endpoint endpoint;                  // 可选项
MESSAGE_END( RegSucceedCommand, hostid, zoneid, hosttype, endpoint )

// 反注册消息
MESSAGE_BEGIN( UnregisterCommand, eSSCommand_Unregister, 1024 )
    Endpoint endpoint; // 可选项
    UnregisterCommand( sid_t id, const Endpoint & ep )
        : SSMessage( eSSCommand_Unregister ) { sid = id; endpoint = ep; }
MESSAGE_END( UnregisterCommand, endpoint )

// 同步服务器时间
MESSAGE_BEGIN( NtpdateCommand, eSSCommand_Ntpdate, 32 )
    int64_t timestamp = 0; // 当前时间
MESSAGE_END( NtpdateCommand, timestamp )

// 发送所有服务器信息
MESSAGE_BEGIN( FrameworkCommand, eSSCommand_Framework, 1024 )
    uint32_t pfid = 0;
    uint8_t machineid = 0;
    std::string appid;                  // appid
    HostID hostid = 0;                  // 主机id
    HostType hosttype = HostType::None; // 主机类型
    ZoneID zoneid = 0;                  // 区服id
    ZoneMode zonemode = ZoneMode::Mix;  // 区服模式
    ZoneIDList mergezonelist;           // 进本服的区服列表
    int64_t starttime = 0;              // 开服日期, UnixTimestamp
    int32_t threadcount = 0;            // 线程个数
    int32_t sesstimeout = 0;            // 会话超时时间
    HostTypes connectlist;              // 连接列表
    HostEndpoints endpoints;            // 所有服务器信息
MESSAGE_END( FrameworkCommand, pfid, machineid, appid, hostid, hosttype, zoneid, zonemode, mergezonelist, starttime, threadcount, sesstimeout, connectlist, endpoints )

// 重新加载信号
MESSAGE_BEGIN( ReloadCommand, eSSCommand_Reload, 64 )
    uint32_t mode = 0;      // 是否强更 0-不强更;1-强更
    uint32_t transid = 0;   // 事务ID
MESSAGE_END( ReloadCommand, mode, transid )

// 系统命令
MESSAGE_BEGIN( ShellCommand, eSSCommand_ShellCommand, 1024 );
    enum {
        CLIENT = 0, // 来自聊天频道
        SERVER = 1, // 来自Master服务器
    };
    uint8_t method = SERVER;
    int32_t transid = 0; // 事务ID
    UnitID roleid = 0;   // 角色ID
    std::string content; // 内容
MESSAGE_END( ShellCommand, method, transid, roleid, content )

// 同步角色状态
MESSAGE_BEGIN( RoleStatusCommand, eSSCommand_RoleStatus, 1024 )
    UnitID roleid = 0; // 角色ID
    int32_t type = 0;  // 状态类型
    bool onoff = 0; // 打开或者关闭
MESSAGE_END( RoleStatusCommand, roleid, type, onoff )

// 查询服务器状态
MESSAGE_BEGIN( ServerStatusCommand, eSSCommand_ServerStatus, 1024 )
    TransID transid = 0;            // 事务id
    HostID id = 0;                  // serverid
    HostType type = HostType::None; // servertype
    std::string status;             // 状态数据
MESSAGE_END( ServerStatusCommand, transid, id, type, status )

// 调试账号
MESSAGE_BEGIN( DebugAccountCommand, eSSCommand_DebugAccount, 1024 )
    std::string account;
    std::string password;
    uint32_t debugseconds = 0;
MESSAGE_END( DebugAccountCommand, account, password, debugseconds )

// 隔天信号
MESSAGE_BEGIN( SkipdayCommand, eSSCommand_Skipday, 32 )
    int64_t timestamp = 0;
MESSAGE_END( SkipdayCommand, timestamp )

// 保存文件
MESSAGE_BEGIN( Save2DocumentCommand, eSSCommand_Save2Document, 4096 )
    std::string path;
    std::string document;
MESSAGE_END( Save2DocumentCommand, path, document )

