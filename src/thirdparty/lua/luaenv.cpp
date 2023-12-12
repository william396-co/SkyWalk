
#include <cstdio>
#include <cassert>
#include <cstring>

#include "base/base.h"

#include "library.h"
#include "luaenv.h"
#include "utils/file.h"
#include "utils/xtime.h"

bool ILuaEnv::addpath( kaguya::State * state, int32_t type, const std::string & path )
{
    std::string newpath;
    std::string suffix, rawpath;

    if ( path.empty() ) {
        return false;
    }

    if ( type == 1 ) {
        suffix = "?.lua";
        rawpath = "package.path";
    } else if ( type == 2 ) {
        suffix = "?.so";
        rawpath = "package.cpath";
    }

    newpath += rawpath;
    newpath += " = ";
    newpath += rawpath;
    newpath += " .. \"";
    newpath += ";";
    newpath += path;

    if ( *path.rbegin() != '/' ) {
        newpath += "/";
    }

    newpath += suffix;
    newpath += "\" ";

    return state->dostring( newpath );
}

ILuaEnv::ILuaEnv( const std::string & path )
    : m_Routine( path ),
      m_State( nullptr )
{}

ILuaEnv::~ILuaEnv()
{}

bool ILuaEnv::initialize()
{
    m_State = new kaguya::State;
    if ( m_State == nullptr ) {
        LOG_ERROR( "ILuaEnv::initialize() : new kaguya::State failed .\n" );
        return false;
    }

    // 注册panic函数
    m_State->setErrorHandler( exceptions );
    lua_atpanic( m_State->state(), ILuaEnv::panic );

    // 获取ROOT
    std::string routine_file;
    utils::PathUtils( m_Routine ).split( m_RootPath, routine_file );

    // 注册
    initEnviroment();
    // 加载入口文件
    m_State->dofile( m_Routine );

    // 钩子
    hook();

    return true;
}

void ILuaEnv::finalize()
{
    unhook();

    if ( m_State ) {
        delete m_State;
        m_State = nullptr;
    }
}

void ILuaEnv::gc2()
{
    m_State->gc();
}

size_t ILuaEnv::memusage()
{
    size_t usage = 0ULL;

    usage = lua_gc( m_State->state(), LUA_GCCOUNT, 0 );
    usage <<= 10ULL;
    usage += lua_gc( m_State->state(), LUA_GCCOUNTB, 0 );

    return usage;
}

void ILuaEnv::dofile( const std::string & file )
{
    m_State->dofile( file );
}

void ILuaEnv::dostring( const std::string & script )
{
    m_State->dostring( script );
}

bool ILuaEnv::addpath( const std::string & path )
{
    return addpath( m_State, 1, path )
            && addpath( m_State, 2, path );
}

int32_t ILuaEnv::panic( lua_State * L )
{
    (void)L; //to avoid warnings */
    LOG_FATAL( "PANIC: unprotected error in call to Lua API ({}) .\n", lua_tostring( L, -1 ) );
    return 0;
}

void ILuaEnv::exceptions( int code, const char * error )
{
    printf( "ILuaEnv::exceptions(Code:%u, Error:'%s') .\n", code, error );
    LOG_ERROR( "ILuaEnv::exceptions(Code:{}, Error:'{}') .\n", code, error );
}

void ILuaEnv::logger( int32_t level, const char * buffer )
{
    LOGGER( level, buffer );
}

void ILuaEnv::initEnviroment()
{
    // 添加ROOT
    addpath( m_RootPath );

    // 初始化其他函数
    kaguya::LoadLibs libs {
        { "_G", luaopen_base },
        { "LUA_MATHLIBNAME", luaopen_math },
        { "LUA_STRLIBNAME", luaopen_string },
        { "pb", luaopen_pb },
        { "vec3", luaopen_vec3 },
        { "rapidjson", luaopen_rapidjson },
        { "fixpoint", luaopen_fixpoint },
    };
    m_State->openlibs( libs );

    // 定义全局变量
    ( *m_State )["ENV"] = this;
    ( *m_State )["ROOT"] = m_RootPath;
    // 定义全局函数
    ( *m_State )["logger"] = ILuaEnv::logger;
    ( *m_State )["traverse"] = ILuaEnv::traverse;
    ( *m_State )["time"] = utils::time;
    ( *m_State )["now"] = utils::now;
    ( *m_State )["current"] = utils::current;
    ( *m_State )["getcwd"] = utils::PathUtils::getcwd;

    // 注册其他接口
    registering();
}

void ILuaEnv::helloworld( int32_t n )
{
    LOG_DEBUG( "ILuaEnv::helloworld() : ------ [global.lua]'s GlobalValue={} ------\n", n );
}

std::vector<std::string> ILuaEnv::traverse( const std::string & path, const std::string & suffix, bool recursive )
{
    std::vector<std::string> files;
    utils::PathUtils(path).traverse(
            files, suffix.empty() ? nullptr : suffix.c_str(), recursive );
    return files;
}

void ILuaEnv::reload( const std::set<std::string> & filelist )
{
    std::string scripts;

    scripts = "local hotfix=require('hotfix'); ";
    for ( const auto & file : filelist ) {
        std::string path, name, base, suffix;
        utils::PathUtils( file ).split( path, name );
        utils::PathUtils( name ).splitext( base, suffix );
        if ( suffix == "lua" ) {
            LOG_TRACE( "LuaEnv::reload('{}') ...\n", file );
            if ( path.find_first_of( root() ) != 0 ) {
                scripts += fmt::format( "require('{0}.{1}'); hotfix:reload('{0}.{1}');", path, base );
            } else {
                if ( path.size() == root().size() ) {
                    scripts += fmt::format( "require('{0}'); hotfix:reload('{0}');", base );
                } else {
                    std::string rpath = path.substr( root().size() + 1 );
                    scripts += fmt::format( "require('{0}.{1}'); hotfix:reload('{0}.{1}');", rpath, base );
                }
            }
        }
    }
    dostring( scripts.c_str() );
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

LuaEnvCenter::LuaEnvCenter()
    : m_Flag( eLuaEnvCenter_Normal ),
      m_UsedIndex( 1 ),
      m_LoadIndex( 0 )
{
    m_Envs[0] = nullptr;
    m_Envs[1] = nullptr;

    pthread_cond_init( &m_Cond, nullptr );
    pthread_mutex_init( &m_Mutex, nullptr );
}

LuaEnvCenter::~LuaEnvCenter()
{
    m_LoadIndex = 0;
    m_UsedIndex = 0;
    m_Flag = eLuaEnvCenter_Normal;
    pthread_cond_destroy( &m_Cond );
    pthread_mutex_destroy( &m_Mutex );

    for ( size_t i = 0; i < 2; ++i ) {
        if ( m_Envs[i] != nullptr ) {
            delete m_Envs[i];
            m_Envs[i] = nullptr;
        }
    }
}

bool LuaEnvCenter::initialize( ILuaEnv * env )
{
    bool rc = false;

    // 初始化
    m_Envs[0] = env;
    m_Envs[1] = env->clone();

    m_Flag = eLuaEnvCenter_Loading;
    rc = getLoad()->initialize();
    m_Flag = eLuaEnvCenter_Rotate;

    if ( rc ) {
        rotate();
        return true;
    }

    return false;
}

void LuaEnvCenter::finalize()
{
    for ( size_t i = 0; i < 2; ++i ) {
        if ( m_Envs[i] != nullptr ) {
            m_Envs[i]->finalize();
        }
    }
}

bool LuaEnvCenter::reload()
{
    pthread_mutex_lock( &m_Mutex );
    if ( m_Flag != eLuaEnvCenter_Normal ) {
        pthread_mutex_unlock( &m_Mutex );
        return false;
    }
    m_Flag = eLuaEnvCenter_Loading;
    pthread_mutex_unlock( &m_Mutex );

    bool rc = getLoad()->initialize();
    if ( !rc ) {
        // 加载失败销毁加载的环境
        getLoad()->finalize();
        m_Flag = eLuaEnvCenter_Normal;
        return false;
    }

    // 等待切换
    // 正常的情况下待逻辑线程切换
    pthread_mutex_lock( &m_Mutex );
    m_Flag = eLuaEnvCenter_Rotate;
    while ( m_Flag != eLuaEnvCenter_Normal ) {
        pthread_cond_wait( &m_Cond, &m_Mutex );
    }
    pthread_mutex_unlock( &m_Mutex );

    // 清空load环境
    getLoad()->finalize();

    return true;
}

ILuaEnv * LuaEnvCenter::env()
{
    return getUsed();
}

void LuaEnvCenter::rotate()
{
    if ( m_Flag != eLuaEnvCenter_Rotate ) {
        return;
    }

    pthread_mutex_lock( &m_Mutex );

    m_Flag = eLuaEnvCenter_Normal;
    std::swap( m_UsedIndex, m_LoadIndex );
    pthread_cond_signal( &m_Cond );

    pthread_mutex_unlock( &m_Mutex );
}

ILuaEnv * LuaEnvCenter::getUsed()
{
    assert( m_UsedIndex < 2 && "LuaEnvCenter UsedIndex is Invalid" );
    assert( m_UsedIndex != m_LoadIndex && "LuaEnvCenter Index is Same" );

    return m_Envs[m_UsedIndex];
}

ILuaEnv * LuaEnvCenter::getLoad()
{
    assert( m_LoadIndex < 2 && "LuaEnvCenter LoadIndex is Invalid" );
    assert( m_UsedIndex != m_LoadIndex && "LuaEnvCenter Index is Same" );

    return m_Envs[m_LoadIndex];
}
