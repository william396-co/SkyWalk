
#ifndef __SRC_LUA_LUAENV_H__
#define __SRC_LUA_LUAENV_H__

#include <string>

#include <pthread.h>

#include "kaguya.hpp"
#include "base/types.h"

//
// Lua环境
//
class ILuaEnv
{
public:
    ILuaEnv( const std::string & path );
    virtual ~ILuaEnv();
    // 钩子(替换lua全局方法)
    virtual void hook() = 0;
    virtual void unhook() = 0;
    // 克隆
    virtual ILuaEnv * clone() = 0;
    // 注册
    virtual void registering() = 0;

    // 添加lua路径
    static bool addpath(
            kaguya::State * state,
            int32_t type, const std::string & path );

public:
    // 初始化/销毁
    bool initialize();
    void finalize();

    // 强制gc
    void gc2();
    size_t memusage();

    // lua栈和kaguya栈
    kaguya::State & state() const { return *m_State; }
    lua_State * luastate() const { return m_State->state(); }
    // 根目录以及入口脚本
    const std::string & root() const { return m_RootPath; }
    const std::string & routine() const { return m_Routine; }

public:
    // 添加路径
    // package.path
    // package.cpath
    bool addpath( const std::string & p );

    // 调用文件
    void dofile( const std::string & file );
    void dostring( const std::string & script );

    // 重新加载
    void reload( const std::set<std::string> & filelist );

protected:
    // 获取当前目录
    static std::string getcwd();
    // 注册
    void initEnviroment();
    // 测试函数
    void helloworld( int32_t n );
    // lua恐慌函数
    static int32_t panic( lua_State * L );
    // 异常函数
    static void exceptions( int code, const char * error );
    // 日志接口
    static void logger( int32_t level, const char * buffer );
    // 遍历文件
    static std::vector<std::string> traverse(
            const std::string & path, const std::string & suffix, bool recursive );

protected:
    std::string m_Routine;   // 入口文件
    std::string m_RootPath;  // 根目录
    kaguya::State * m_State; // kaguya状态机
};

//
// Lua环境中心
// 支持脚本热更新
//
class LuaEnvCenter
{
public:
    LuaEnvCenter();
    ~LuaEnvCenter();

    // 初始化和销毁
    bool initialize( ILuaEnv * env );
    void finalize();
    // 热更新脚本环境
    // 会阻塞到环境切换成功
    bool reload();
    // 如有需要, 切换环境
    void rotate();
    // 获取当前正在使用的脚本环境
    ILuaEnv * env();

private:
    enum
    {
        eLuaEnvCenter_Normal = 1,
        eLuaEnvCenter_Loading = 2,
        eLuaEnvCenter_Rotate = 3
    };

    ILuaEnv * getUsed();
    ILuaEnv * getLoad();

private:
    uint8_t m_Flag;
    uint8_t m_UsedIndex;
    uint8_t m_LoadIndex;

    pthread_cond_t m_Cond;
    pthread_mutex_t m_Mutex;
    ILuaEnv * m_Envs[2];
};

#endif
