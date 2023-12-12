
#include <cassert>
#include <cstring>

#include <unistd.h>
#include <pthread.h>

#include "configcenter.h"

namespace utils {

ConfigCenter::ConfigCenter()
    : m_Flag( eConfigCenter_Normal ),
      m_UsedIndex( 1 ),
      m_LoadIndex( 0 )
{}

ConfigCenter::~ConfigCenter()
{
    m_LoadIndex = -1;
    m_UsedIndex = -1;
    m_Flag = eConfigCenter_Normal;
}

ConfigCenter * ConfigCenter::addPath( const char * path )
{
    m_SearchPaths.push_back( path );
    return this;
}

bool ConfigCenter::load()
{
    ConfigContainer * container = getLoad();

    // 加载
    m_Flag = eConfigCenter_Loading;

    auto it = container->begin();
    for ( ; it != container->end(); ++it ) {
        IConfigfile * configfile = it->second;
        std::string path = getRealPath( it->first );

        configfile->filepath( path );
        if ( !configfile->load( path.c_str() ) ) {
            break;
        }
    }
    m_Flag = eConfigCenter_Rotate;

    // 加载成功
    if ( it == container->end() ) {
        rotate();
        return true;
    }

    return false;
}

bool ConfigCenter::reloadInThread( const char * filename )
{
    // 加载
    for ( const auto & p : *getUsed() ) {
        IConfigfile * file = p.second;
        std::string path = getRealPath( p.first );

        if ( filename != nullptr
            && strcmp( filename, p.first.c_str() ) != 0 ) {
            continue;
        }

        // 卸载
        file->unload();
        // 加载
        file->filepath( path );
        file->load( path.c_str() );
    }

    return true;
}

bool ConfigCenter::reload()
{
    // 设置标志
    {
        std::unique_lock<std::mutex> guard( m_RotateLock );
        if ( m_Flag != eConfigCenter_Normal ) {
            return false;
        }
        m_Flag = eConfigCenter_Loading;
    }

    // 加载
    auto it = getLoad()->begin();
    for ( ; it != getLoad()->end(); ++it ) {
        IConfigfile * file = it->second;
        std::string path = getRealPath( it->first );

        file->filepath( path );
        if ( !file->load( path.c_str() ) ) {
            break;
        }
    }

    // 重新加载配置出错
    if ( it != getLoad()->end() ) {
        for ( const auto & p : *getLoad() ) {
            if ( p.second != nullptr ) {
                p.second->unload();
                p.second->filepath( "" );
            }
        }
        m_Flag = eConfigCenter_Normal;
        return false;
    }

    // 等待主线程轮换
    {
        std::unique_lock<std::mutex> guard( m_RotateLock );
        m_Flag = eConfigCenter_Rotate;
        m_RotateCond.wait( guard,
            [this]() { return m_Flag == eConfigCenter_Normal; } );
    }

    // 卸载
    for ( const auto & p : *getLoad() ) {
        if ( p.second ) {
            p.second->unload();
            p.second->filepath( "" );
        }
    }

    return true;
}

void ConfigCenter::unload()
{
    for ( int8_t i = 0; i < 2; ++i ) {
        for ( const auto & p : m_Container[i] ) {
            if ( p.second ) {
                p.second->unload();
                p.second->filepath( "" );
                delete p.second;
            }
        }
    }
}

void ConfigCenter::rotate()
{
    if ( m_Flag != eConfigCenter_Rotate ) {
        return;
    }

    {
        std::unique_lock<std::mutex> guard( m_RotateLock );
        m_Flag = eConfigCenter_Normal;
        std::swap( m_UsedIndex, m_LoadIndex );
    }
    m_RotateCond.notify_one();

    // 加载成功回调
    if ( m_ReloadCallback != nullptr ) {
        m_ReloadCallback();
    }

    return;
}

bool ConfigCenter::add( const char * path, const std::function<IConfigfile *()> & file, bool ignore )
{
    ConfigContainer * loads = getLoad();
    ConfigContainer * useds = getUsed();

    assert( path != nullptr && "path == nullptr" );
    assert( file != nullptr && "file == nullptr" );

    // 检查文件是否存在
    if ( ignore && getRealPath( path ).empty() ) {
        return false;
    }

    // 查找配置是否存在
    assert( loads->find( path ) == loads->end() && "config file is existed" );
    assert( useds->find( path ) == useds->end() && "config file is existed" );

    loads->emplace( path, file() );
    useds->emplace( path, file() );

    return true;
}

IConfigfile * ConfigCenter::get( const char * path, bool used )
{
    ConfigContainer * container = nullptr;
    if ( used ) {
        container = getUsed();
    } else {
        container = getLoad();
    }

    auto result = container->find( path );
    if ( result == container->end() ) {
        return nullptr;
    }

    return result->second;
}

ConfigCenter::ConfigContainer * ConfigCenter::getUsed()
{
    assert( m_UsedIndex != m_LoadIndex && "Index is Same" );
    assert( ( m_UsedIndex == 0 || m_UsedIndex == 1 ) && "the UsedIndex is invalid" );
    return &( m_Container[m_UsedIndex] );
}

ConfigCenter::ConfigContainer * ConfigCenter::getLoad()
{
    assert( m_UsedIndex != m_LoadIndex && "Index is Same" );
    assert( ( m_LoadIndex == 0 || m_LoadIndex == 1 ) && "the LoadIndex is invalid" );
    return &( m_Container[m_LoadIndex] );
}

std::string ConfigCenter::getRealPath( const std::string & file ) const
{
    assert( !m_SearchPaths.empty() && "SearchPath is EMPTY" );

    for ( auto it = m_SearchPaths.rbegin(); it != m_SearchPaths.rend(); ++it ) {
        std::string path = ( *it ) + "/" + file;

        if ( ::access( path.c_str(), F_OK ) == 0 ) {
            return path;
        }
    }

    return "";
}

} // namespace utils
