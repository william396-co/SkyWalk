#pragma once
#include <map>
#include <vector>
#include <string>
#include <cstdio>
#include <cstdint>

#ifndef WIN32
#include <pthread.h>
#else
#include <thread>
#endif

#include "utils/slice.h"
#include "utils/types.h"

namespace utils {

//
// 文件工具
//
class FileUtils
{
public:
    // 打开pidfile
    // 写入进程号
    // 并返回打开的文件描述符
    static int32_t pidfopen( const std::string & file );

    // 加锁写文件
    static bool writelocked(
        const std::string & file, const std::string & buffer );

    // 获取文件大小
    static ssize_t getFileSize( const std::string & path );

    // 获取文件内容
    static bool getcontent( const std::string & path, std::string & content );
    // 获取文件的行
    static bool getlines( const std::string & path, std::vector<std::string> & lines );
};

//
// 目录工具
//
class PathUtils
{
public:
    enum Type
    {
        NONE = 0, // 不存在
        FILE = 1, // 文件
        PATH = 2, // 目录
        LINK = 3, // 链接
    };

    PathUtils( const std::string & path )
        : m_Path( path )
    {}

    ~PathUtils()
    {}

public:
    // 当前工作目录
    static std::string getcwd();

    // 建立多级目录
    // 模拟mkdir -p的功能
    static bool mkdirp( const std::string & p );

    // 目录类型
    Type type() const;

    // 检查后缀名
    static inline bool isuffix( const char * path, const char * suffix );

    // 分割目录和文件
    bool split( std::string & dir, std::string & base );
    // 分割文件名和后缀名
    bool splitext( std::string & base, std::string & suffix );

    // 读取目录
    bool traverse( std::vector<std::string> & files,
            const char * suffix = nullptr, bool recursive = false );
    bool traverse( std::vector<std::string> & files,
            std::vector<std::string> & paths,
            const char * suffix = nullptr, bool recursive = false );

private:
    std::string m_Path;
};

//
//
//
class WritableFile
{
public:
    WritableFile( const std::string & path, size_t cachesize = 0 );
    ~WritableFile();
    bool append( const Slice & data );
    bool append( const std::string & data );
    bool close();
    bool flush();
    bool sync();
    size_t erase();
    const std::string & path() const { return m_Path; }

private:
    std::string m_Path;
    FILE * m_File;
    std::string m_Cache;
};

//
// 配置文件, INI文件格式
//
class ConfigSection;
class ConfigFile
{
public:
    ConfigFile( const char * path );
    ~ConfigFile();

public:
    // 打开配置文件
    bool open();
    // 关闭配置文件
    void close();

public:
    //
    template<class T> bool get( const char * section, const char * key, T & value ) const;
    template<class T>
        bool get( const char * section, const char * key, std::vector<T> & value ) const {
            const char * _value = getValue( section, key );
            if ( _value != nullptr ) {
                char *word, *brkt;
                const char * sep = ",";
                char * belist = strdup( _value );
                for ( word = strtok_r( belist, sep, &brkt );
                      word;
                      word = strtok_r( nullptr, sep, &brkt ) ) {
                    value.push_back( T( std::atoll( word ) ) );
                }
                std::free( belist );
                return true;
            }
            return false;
        }

    // 检查是否有该主键
    bool has( const char * section, const char * key ) const;

    // 获取节信息
    bool get( const char * section, std::vector<std::string> & keylist ) const;

    // 获取section-key-value
    bool get( const char * section, const char * key, bool & value ) const;
    bool get( const char * section, const char * key, float & value ) const;
    bool get( const char * section, const char * key, int8_t & value ) const;
    bool get( const char * section, const char * key, uint8_t & value ) const;
    bool get( const char * section, const char * key, int16_t & value ) const;
    bool get( const char * section, const char * key, uint16_t & value ) const;
    bool get( const char * section, const char * key, int32_t & value ) const;
    bool get( const char * section, const char * key, uint32_t & value ) const;
    bool get( const char * section, const char * key, int64_t & value ) const;
    bool get( const char * section, const char * key, uint64_t & value ) const;
    bool get( const char * section, const char * key, std::string & value ) const;

#if defined( __APPLE__ ) || defined( __darwin__ )
    bool get( const char * section, const char * key, long & value ) const;
    bool get( const char * section, const char * key, unsigned long & value ) const;
#endif

private:
    // 解析配置文件
    void parse( char * filecontent, int32_t filesize );

    // 获取指定section指定key的value值
    const char * getValue( const char * section, const char * key ) const;

private:
    typedef std::map<std::string, ConfigSection *> SectionMap;

    std::string m_File;
    SectionMap m_Sections;
};

//
// 日志文件
//
class Logger;
class CShmem;
class LoggerLock;

class LogFile
{
public:
    // 日志级别
    enum
    {
        eLogLevel_Fatal = 1, // 严重
        eLogLevel_Error = 2, // 错误
        eLogLevel_Warn = 3,  // 警告
        eLogLevel_Info = 4,  // 信息
        eLogLevel_Trace = 5, // 跟踪
        eLogLevel_Debug = 6, // 调试
    };

    LogFile( const char * path, const char * module );
    ~LogFile();

public:
    // 打开日志文件
    bool open();

    // 打印日志
    // 将format中的内容直接输出到日志中
    void print( uint8_t level, const char * format, ... );
    void print( uint8_t level, const std::string & content );

    // 打印日志
    // 定前缀+format中的内容一并输出到日志中
    // %t - 年月日 时分秒
    // %T - 年月日 时分秒,毫秒
    // %l %L - 日志等级
    // %p %P - 线程ID
    // %f %F - 文件+line
    void printp( uint8_t level, const char * prefix, const char * format, ... );
    void printp( uint8_t level, const char * prefix, const std::string & content );
    void printp( uint8_t level, const char * prefix, const Sourcefile & f, const char * format, ... );
    void printp( uint8_t level, const char * prefix, const Sourcefile & f, const std::string & content );

    // 强制刷新日志文件
    // 为了保证数据刷新到文件中, 所以该接口效率比较低
    void flush();

    // 关闭日志文件
    void close();

    // 日志的相关设置
    void setMaxSize( size_t size );
    void setLevel( uint8_t level = 0 );

private:
    friend class Logger;

    // 打印日志
    void append( uint8_t level,
        int32_t today, const std::string & body );

    // 确保key文件存在
    void ensure_keyfiles_exist( char * file1, char * file2 );

    CShmem * getBlock() const { return m_Block; }
    const char * getPath() const { return m_Path.c_str(); }
    const char * getModule() const { return m_Module.c_str(); }

private:
    std::string m_Path;
    std::string m_Module;
    CShmem * m_Block;    // 共享内存块
    LoggerLock * m_Lock; // 锁
    Logger * m_Logger;   // 日志
};

} // namespace utils
