#pragma once

#include <unordered_map>
#include <map>
#include <vector>
#include <string>
#include <cstdint>
#include <mutex>
#include <functional>
#include <condition_variable>

#include "types.h"
#include <stdint.h>

namespace utils {

class IConfigfile
{
public:
    IConfigfile() {}
    virtual ~IConfigfile() {}
    virtual void unload() = 0;
    virtual bool load( const char * path ) = 0;
    const std::string & filepath() const { return m_Filepath; }
    void filepath( const std::string & p ) { m_Filepath = p; }

private:
    std::string m_Filepath;
};

//
// 配置中心
//
class ConfigCenter
{
public:
    ConfigCenter();
    ~ConfigCenter();

public:
    // 加载, 卸载
    bool load();
    void unload();

    // 当前线程中重新加载配置文件
    bool reloadInThread( const char * filename = nullptr );

    // 重新加载
    // 阻塞至rotate()完成
    bool reload();
    // 轮换
    void rotate();

    // 获取配置文件
    // 默认获取使用中的配置文件
    IConfigfile * get( const char * path, bool used = true );

    // 添加搜索路径
    // 添加的顺序决定的优先级, 后添加的优先级高
    ConfigCenter * addPath( const char * path );

    // 增加配置文件
    // ignore - 是否忽略不存在的文件
    bool add( const char * path, const std::function<IConfigfile *()> & file, bool ignore = false );

    // 获取真实路径
    std::string getRealPath( const std::string & file ) const;

    // 注册回调
    void regCallback( const std::function<void()> & fn ) { m_ReloadCallback = fn; }

private:
    enum
    {
        eConfigCenter_Normal = 1,  // 正常
        eConfigCenter_Loading = 2, // 加载中...
        eConfigCenter_Rotate = 3,  // 需要交换
    };

    using ConfigContainer = std::unordered_map<std::string, IConfigfile *>;

    ConfigContainer * getUsed();
    ConfigContainer * getLoad();

private:
    // 当前使用/加载的容器索引
    int8_t m_Flag;
    int8_t m_UsedIndex;
    int8_t m_LoadIndex;
    std::vector<std::string> m_SearchPaths; // 搜索路径

    std::mutex m_RotateLock;
    std::condition_variable m_RotateCond;
    ConfigContainer m_Container[2];
    std::function<void ()> m_ReloadCallback;
};

} // namespace utils

