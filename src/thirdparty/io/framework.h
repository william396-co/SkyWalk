
#ifndef __SRC_IO_FRAMEWORK_H__
#define __SRC_IO_FRAMEWORK_H__

#include <vector>
#include <string>
#include <mutex>
#include <cstdint>

#include "base/types.h"
#include "base/endpoint.h"
#include "utils/types.h"

namespace utils {
class ConfigFile;
}

//
// 框架图相关
//
class Framework
{
public:
    Framework();
    ~Framework();

    // 清空(全局服务器除外)
    void clear( bool isglobal );
    // 忽略某种全局服务器
    const HostTypes & ignore() const { return m_IgnoreTypeList; }
    void ignore( const HostTypes & types ) { m_IgnoreTypeList = types; }

    //
    // 获取服务器信息(线程安全的版本)
    // onConnectSucceed()回调中调用
    //
    bool has( const Endpoint & ep );

    // 获取服务器信息(线程安全版本)
    bool get( HostID id, HostEndpoint & endpoint );

public:
    // 获取服务器信息
    const HostEndpoint * get( HostID id ) const;
    const HostEndpoint * get( const Endpoint & endpoint ) const;

    // 根据HostID获取类型
    HostType getType( HostID id ) const;

    // 获取服务器索引(从1开始)
    uint32_t getIndex( HostID id ) const;

    // 获取指定类型的服务器ID
    uint32_t getHostCount( HostType type ) const;
    HostIDs getHostIDs( HostType type ) const;
    void getHostIDs( HostType type, HostIDs & idlist ) const;

    // 根据类型获取远端地址列表
    bool getEndpoint( HostID id, Endpoint & ep ) const;
    const HostEndpoint * getEndpoint( HostType type ) const;
    void getEndpoints( HostType type, Endpoints & endpoints ) const;
    void getEndpoints( HostType type, HostEndpoints & endpoints ) const;

    // 获取超时时间
    int32_t getThreadCount() const { return m_ThreadCount; }
    int32_t getSessionTimeout() const { return m_SessTimeout; }
    // 获取所有服务器信息
    const HostEndpoints & getHostEndpoints() const { return m_HostEndpoints; }

public:
    // 获取framework信息
    std::string getInformation() const;

    // 设置超时时间
    void setSessTimeout( int32_t seconds ) { m_SessTimeout = seconds; }
    void setThreadCount( int32_t nthreads ) { m_ThreadCount = nthreads; }

    //
    // 更新服务器信息(单区or跨服集群)
    // Slave::initialize()中调用
    // 或者主逻辑线程中调用
    //
    void updateLocalHosts( const HostEndpoints & hosts );
    void updateGlobalHosts( const HostEndpoints & hosts );

private:
    friend class ISysConfigfile;

private:
    std::mutex m_Lock;      // 锁
    int32_t m_ThreadCount;         // 线程个数
    int32_t m_SessTimeout;         // 超时时间
    HostTypes m_IgnoreTypeList;    // 忽略的类型列表
    HostEndpoints m_HostEndpoints; // 主机
                                   // Global服务器是配置文件中的
                                   // Master服务器是配置文件中的+GlobalServer下发的
                                   // 其他服务器，都是Global或者Master下发的
};

//
// 基础配置定义
//
class ISysConfigfile
{
public:
    ISysConfigfile( HostType type, const std::string & file )
        : m_HostType( type ),
          m_Configfile( file ),
          m_Status( 0 ),
          m_ServerID( 0 ),
          m_LogLevel( 0 ),
          m_LogFilesize( 0 ),
          m_PfID( 0 ),
          m_ZoneID( 0 ),
          m_ZoneMode( ZoneMode::Mix ),
          m_StartTime( 0 )
    {}

    virtual ~ISysConfigfile()
    {}

    virtual bool load() = 0;
    virtual bool reload() = 0;
    virtual void unload() = 0;

public:
    // 解析Framework or Zone节
    bool parse();
    void cleanup();

    // 获取框架图
    Framework * getFramework() { return &m_Framework; }
    // 获取配置
    const std::string & getConfigfile() const { return m_Configfile; }

    // 获取/设置服务器ID
    HostID getServerID() const { return m_ServerID; }
    void setServerID( HostID id ) { m_ServerID = id; }

    // 获取内外网绑定地址
    const Endpoint * getEndpoint() const;
    const Endpoint * getExtEndpoint() const;
    bool getExtEndpoint( Endpoint & endpoint );

    // 获取Master主机地址
    const Endpoint & getMasterEndpoint() const { return m_MasterEndpoint; }

    // 获取日志等级
    uint8_t getLogLevel() const { return m_LogLevel; }
    // 获取日志文件大小限制
    size_t getLogFilesize() const { return m_LogFilesize; }

    // 获取后端服务器类型
    void setConnectList( const HostTypes & l ) { m_ConnectList = l; }
    const HostTypes & getConnectList() const { return m_ConnectList; }

    // 获取一致性hash索引JCH
    HostID getJchHostID( HostType type, uint64_t key ) const;
    // 获取服务器索引
    uint32_t getServerIndex() const { return m_Framework.getIndex( m_ServerID ); }

public:
    //
    // 区服相关
    //

    // 今天是开服第几天
    int32_t getStartDays( time_t now ) const;

    // 是否是本服管理
    bool isInZone( ZoneID id ) const;
    // 是否是测试服或者是正式服
    bool isBetaZone() const { return m_ZoneMode == ZoneMode::Beta || m_ZoneMode == ZoneMode::Mix; }
    bool isOfficialZone() const { return m_ZoneMode == ZoneMode::Official || m_ZoneMode == ZoneMode::Mix; }

public:
    // 状态
    int32_t getStatus() const { return m_Status; }
    void setStatus( int32_t status ) { m_Status = status; }

    // 平台ID
    uint32_t getPfID() const { return m_PfID; }
    void setPfID( uint32_t id ) { m_PfID = id; }

    // 机房ID
    uint8_t getMachineID() const { return m_MachineID; }
    void setMachineID( uint8_t id ) { m_MachineID = id; }

    // 应用ID
    const std::string & getAppID() const { return m_AppID; }
    void setAppID( const std::string & id ) { m_AppID = id; }

    // 区服ID
    ZoneID getZoneID() const { return m_ZoneID; }
    void setZoneID( ZoneID zoneid ) { m_ZoneID = zoneid; }

    // 区服模式
    ZoneMode getZoneMode() const { return m_ZoneMode; }
    void setZoneMode( ZoneMode zonemode ) { m_ZoneMode = zonemode; }

    // 开服时间
    time_t getStartTime() const { return m_StartTime; }
    void setStartTime( int32_t time ) { m_StartTime = time; }

    // 合服列表
    const ZoneIDList & getMergeList() const { return m_MergeZoneList; }
    void setMergeList( const ZoneIDList & list ) { m_MergeZoneList = list; }

private:
    // 解析终端地址
    void parseEndpoint( HostID id, const std::string & data );

protected:
    Framework m_Framework;
    HostType m_HostType;
    std::string m_Configfile;

protected:
    int32_t m_Status;          // 服务器状态
    HostID m_ServerID;
    uint8_t m_LogLevel;        // MasterProcess中使用
    size_t m_LogFilesize;      // MasterProcess中使用
    HostTypes m_ConnectList;   // Harbor中rebuild()需要使用
    Endpoint m_MasterEndpoint; // Master主机地址

protected:
    uint32_t m_PfID;            // 平台ID
    uint8_t m_MachineID;        // 机房ID
    std::string m_AppID;        // APPID
    ZoneID m_ZoneID;            // 区服ID
    ZoneMode m_ZoneMode;        // 见define.h
    ZoneIDList m_MergeZoneList; // 合并进本服的区服列表
    time_t m_StartTime;         // 开服日期, UnixTimestamp
};

#endif
