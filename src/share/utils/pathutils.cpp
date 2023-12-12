
#include <cstring>
#include <cstdlib>

#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <dirent.h>

#include "file.h"

namespace utils {

std::string PathUtils::getcwd()
{
    char path[1024];
    ::getcwd( path, 1023 );
    return path;
}

bool PathUtils::mkdirp( const std::string & path )
{
    int32_t len = std::strlen( path.c_str() );
    char * p = (char *)std::malloc( len + 2 );

    if ( p == nullptr ) {
        return false;
    }

    std::strcpy( p, path.c_str() );

    if ( p[len - 1] != '/' ) {
        p[len] = '/';
        len = len + 1;
    }

    for ( int32_t i = 1; i < len; ++i ) {
        if ( p[i] != '/' ) {
            continue;
        }

        p[i] = 0;

        if ( ::access( p, F_OK ) != 0 ) {
            ::mkdir( p, 0755 );
        }

        p[i] = '/';
    }

    std::free( p );
    return true;
}

PathUtils::Type PathUtils::type() const
{
    struct stat fs;

    if ( ::stat( m_Path.c_str(), &fs ) != 0 ) {
        return NONE;
    }

    if ( S_ISDIR( fs.st_mode ) ) {
        return PATH;
    } else if ( S_ISLNK( fs.st_mode ) ) {
        return LINK;
    }

    return PathUtils::FILE;
}

bool PathUtils::isuffix( const char * path, const char * suffix )
{
    if ( suffix == nullptr ) {
        return true;
    }

    const char * cursuffix = strrchr( path, '.' );
    if ( cursuffix != nullptr
            && strcmp( cursuffix + 1, suffix ) == 0 ) {
        return true;
    }

    return false;
}

bool PathUtils::split( std::string & dir, std::string & base )
{
    switch ( type() ) {
        case PATH:
            dir = m_Path;
            break;

        case FILE: {
            size_t pos = m_Path.rfind( "/" );
            if ( pos == std::string::npos ) {
                return false;
            }
            base = m_Path.substr( pos + 1 );
            dir = m_Path.substr( 0, pos );
        } break;

        case NONE:
            return false;
            break;

        case LINK:
            return false;
            break;
    }

    return true;
}

bool PathUtils::splitext( std::string & base, std::string & suffix )
{
    size_t pos = m_Path.rfind( "." );
    if ( pos == std::string::npos ) {
        return false;
    }

    base = m_Path.substr( 0, pos );
    suffix = m_Path.substr( pos + 1 );

    return true;
}

bool PathUtils::traverse( std::vector<std::string> & files, const char * suffix, bool recursive )
{
    std::vector<std::string> paths;
    return traverse( files, paths, suffix, recursive );
}

bool PathUtils::traverse( std::vector<std::string> & files, std::vector<std::string> & paths, const char * suffix, bool recursive )
{
    struct dirent * file = nullptr;

    // 读取目录
    DIR * dir = opendir( m_Path.c_str() );
    if ( dir == nullptr ) {
        return false;
    }

    if ( PathUtils::isuffix(
                m_Path.c_str(), suffix ) ) {
        paths.push_back( m_Path );
    }

    while ( ( file = readdir( dir ) ) != nullptr ) {
        // 跳过.和..
        if ( strcmp( file->d_name, "." ) == 0
                || strcmp( file->d_name, ".." ) == 0 ) {
            continue;
        }

        std::string current = m_Path;
        current += "/";
        current += file->d_name;

        if ( PathUtils::isuffix( current.c_str(), suffix ) ) {
            if ( file->d_type == DT_REG ) {
                files.push_back( current );
            } else if ( file->d_type == DT_DIR && !recursive ) {
                paths.push_back( current );
            }
        }

        if ( recursive && file->d_type == DT_DIR ) {
            PathUtils(current).traverse( files, paths, suffix, recursive );
        }
    }

    closedir( dir );
    return true;
}

} // namespace utils
