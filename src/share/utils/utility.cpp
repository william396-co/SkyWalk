
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <memory>
#include <algorithm>

#include <unistd.h>
#include <stdarg.h>
#include <cxxabi.h>

#include "utility.h"

namespace utils {

void Utility::trim( std::string & str )
{
    str.erase( 0, str.find_first_not_of( " " ) );
    str.erase( str.find_last_not_of( " " ) + 1 );
}

char * Utility::strsep( char ** s, const char * del )
{
    char *d, *tok;

    if ( !s || !*s ) {
        return nullptr;
    }

    tok = *s;
    d = std::strstr( tok, del );

    if ( d ) {
        *s = d + std::strlen( del );
        *d = '\0';
    } else {
        *s = nullptr;
    }

    return tok;
}

std::string Utility::demangle( const std::string & name )
{
    int status = -4;
    std::unique_ptr<char, void(*)(void *)> res {
        abi::__cxa_demangle(name.c_str(), nullptr, nullptr, &status), std::free
    };
    return status == 0 ? res.get() : name;
}

IpAddress Utility::parseaddress( const std::string & ip )
{
    IpAddress result;

    char * address = strdup( ip.c_str() );
    if ( address != nullptr ) {
        char *word, *brkt;
        const char * sep = ".";
        for ( word = strtok_r( address, sep, &brkt );
              word;
              word = strtok_r( nullptr, sep, &brkt ) ) {
            if ( strcmp( word, "*" ) == 0 ) {
                result.push_back( 256 );
            } else {
                result.push_back( std::atoi( word ) );
            }
        }
        free( address );
    }

    result.resize( 4 );
    return result;
}

std::vector<uint32_t> Utility::parseversion( const std::string & version )
{
    std::vector<uint32_t> verarray;

    char * version_ = strdup( version.c_str() );
    if ( version_ != nullptr ) {
        char *word, *brkt;
        const char * sep = ".";
        for ( word = strtok_r( version_, sep, &brkt );
              word;
              word = strtok_r( nullptr, sep, &brkt ) ) {
            verarray.push_back( std::atoi( word ) );
        }
        free( version_ );
    }

    return verarray;
}

std::vector<std::string> Utility::parsecmdline( const std::string & cmd )
{
    std::vector<std::string> cmds;

    char *word, *brkt;
    const char * sep = " ";
    char * command = ::strdup( cmd.c_str() );
    for ( word = ::strtok_r( command, sep, &brkt );
          word;
          word = ::strtok_r( nullptr, sep, &brkt ) ) {
        cmds.push_back( word );
    }
    std::free( command );

    return cmds;
}

void Utility::randstring( size_t len, std::string & value )
{
    // 清空字符串
    value.clear();

    for ( size_t i = 0; i < len; ++i ) {
        switch ( random_thread_local::get( 0, 2 ) ) {
            case 0: value.push_back( random_thread_local::get( 'a', 'z' ) ); break;
            case 1: value.push_back( random_thread_local::get( 'A', 'Z' ) ); break;
            case 2: value.push_back( random_thread_local::get( '0', '9' ) ); break;
        }
    }
}

int32_t Utility::snprintf( std::string & str, size_t size, const char * format, ... )
{
    str.resize( size );

    va_list args;
    va_start( args, format );
    int32_t rc = vsnprintf(
        const_cast<char *>( str.data() ), size - 1, format, args );
    if ( rc >= 0 && rc <= ( int32_t )( size - 1 ) ) {
        str.resize( rc );
    }
    va_end( args );

    return rc;
}

bool Utility::getExePath( std::string & path )
{
    ssize_t len = 0;
    const std::string deleted( " (deleted)" );

    path.resize( 512 );
    len = readlink(
        "/proc/self/exe", (char *)path.data(), 512 );
    if ( len == -1 ) {
        return false;
    }
    path.resize( len );

    // 删除"/usr/local/bin/flume (deleted)"
    if ( path.size() > deleted.size() ) {
        size_t len = path.size() - deleted.size();

        if ( path.substr( len ) == deleted ) {
            path.resize( len );
        }
    }

    return true;
}

bool Utility::checkbuiltinwords( const char * words )
{
    int32_t zoneid = 0;
    char fullwords[1024] = { 0 };
    std::string in_words = words;

    char * word = (char *)in_words.c_str();
    // 内置字符
    const char * special = " =,\'\"#*?|/\\$%&.";

    ::strsep( &word, special );
    return word != nullptr
        || ::sscanf( in_words.c_str(), "s%d.%s", &zoneid, fullwords ) != 0;
}

} // namespace utils
