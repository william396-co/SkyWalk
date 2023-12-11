#pragma once

#include <vector>
#include <string>

#include <stdint.h>
#include <mysql/mysql.h>

#include "utils/slice.h"
#include "base/database.h"

class MysqlResults;

class IResultHandler
{
public:
    IResultHandler() {}
    virtual ~IResultHandler() {}
    virtual void reserve( size_t capacity ) = 0;
    virtual void process( const Slices & row ) = 0;
};

class MysqlEngine
{
public:
    MysqlEngine( const std::string & host, uint16_t port );
    ~MysqlEngine();

    static void start();
    static void stop();
    // SQL绑定
    static bool sqlbind( std::string & sqlcmd,
        const std::string & from, const std::vector<std::string> & values );
    // escape
    static void escapesqlcmd(
        std::string & sqlcmd, MysqlEngine * engine,
        const std::string & from, const std::vector<std::string> & values );

public:
    // 设置访问权限
    void setToken(
        const std::string & user,
        const std::string & passwd );
    // 选择数据库
    bool selectdb( const std::string & database );
    // 设置字符集
    void setCharsets( const std::string & charsets );
    // 创建数据库
    bool createdb( const std::string & database, const std::string & charsets = "" );

public:
    // 初始化
    bool initialize();
    // 销毁
    void finalize();
    // 保活
    void keepalive();
    // 版本号
    void version( std::string & v ) const;
    // 获取连接
    MYSQL * handler() const { return m_DBHandler; }
    // 错误码
    int32_t errorcode() const { return m_Errorcode; }
    // 包的最大长度
    size_t maxpacketlength() const { return m_MaxPacketLength; }
    // 直接执行sqlcmd
    bool execute( const std::string & sqlcmd );
    // 转义
    void escape( const std::string & src, std::string & dst );
    // 插入
    bool insert( const std::string & sqlcmd, uint64_t & insertid );
    // 查询(返回结果集)
    bool query( const std::string & sqlcmd,
        std::vector<std::vector<std::string>> & results );
    // 查询(内存优化的版本)
    bool query( const std::string & sqlcmd, MysqlResults * results );
    // 查询(单行回调)
    bool query( const std::string & sqlcmd, IResultHandler * handler );
    // 更新
    bool update( const std::string & sqlcmd, uint32_t & naffected );
    // 删除
    bool remove( const std::string & sqlcmd, uint32_t & naffected );
    // 替换
    bool replace( const std::string & sqlcmd, uint64_t & insertid, uint32_t & naffected );

private:
    enum
    {
        eMysqlEngine_ConnectTimeout = 5, // 连接超时时间
        eMysqlEngine_ReadTimeout = 5,    // 读超时时间
        eMysqlEngine_WriteTimeout = 5,   // 写超时时间
    };

    static int64_t now();
    ssize_t queryMaxPacketLength();

private:
    std::string m_Host;     // 主机地址
    uint16_t m_Port;        // 端口号
    std::string m_Username; // 用户名
    std::string m_Password; // 密码
    std::string m_Database; // 数据库名
    std::string m_Charsets; // 字符集

private:
    int32_t m_Errorcode;      // 错误码
    MYSQL * m_DBHandler;      // 数据库句柄
    size_t m_MaxPacketLength; // 最大包的长度
};
