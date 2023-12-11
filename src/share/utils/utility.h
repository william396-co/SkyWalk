#pragma once

#include <vector>
#include <string>
#include <cassert>

#include <stddef.h>
#include <stdint.h>

#include "types.h"
#include "random.h"

namespace utils {

class Utility
{
public:
    // 去除空格
    static void trim( std::string & str );

    // strsep
    static char * strsep( char ** s, const char * del );

    // name demangle
    static std::string demangle( const std::string & name );

    // 解析ip地址
    static IpAddress parseaddress( const std::string & ip );
    // 解析版本号
    static std::vector<uint32_t> parseversion( const std::string & version );
    // 解析命令行
    static std::vector<std::string> parsecmdline( const std::string & cmd );

    // 随机
    template<class T>
        static const T & random( const std::vector<T> & array )
        {
            assert( !array.empty() && "Empty Array" );
            return *random_thread_local::get( array );
        }

    // 随机排序
    template<class T>
        static void shuffle( std::vector<T> & array )
        {
            random_thread_local::shuffle( array );
        }

    // 根据权重随机
    template<class InputIt, class GetWeight>
        static InputIt random( InputIt first, InputIt last, GetWeight f )
        {
            size_t rates = 0, until = 0, total = 0;
            for ( InputIt it = first; it != last; ++it ) {
                total += f( *it );
            }
            if ( total != 0 ) {
                rates = random_thread_local::get<size_t>( 1, total );
                for ( InputIt it = first; it != last; ++it ) {
                    until += f( *it );
                    if ( rates <= until ) {
                        return it;
                    }
                }
            }
            return last;
        }

    // 差集
    template<class T>
        static void difference( const std::vector<T> & olds, const std::vector<T> & news, std::vector<T> & adds, std::vector<T> & dels )
        {
            for ( auto id : olds ) {
                if ( news.end()
                    == std::find( news.begin(), news.end(), id ) ) {
                    dels.push_back( id );
                }
            }
            for ( auto id : news ) {
                if ( olds.end()
                    == std::find( olds.begin(), olds.end(), id ) ) {
                    adds.push_back( id );
                }
            }
        }

    template<class T>
        static void append( std::vector<T> & array, const T & element )
        {
            if ( array.end()
                == std::find( array.begin(), array.end(), element ) ) {
                array.push_back( element );
            }
        }

    // 随机字符串
    static void randstring( size_t len, std::string & value );

    // 字符串sprintf
    static int32_t snprintf( std::string & dst, size_t size, const char * format, ... );

    // 获取可执行文件全路径
    static bool getExePath( std::string & path );

    // 检查是否是内建字串
    static bool checkbuiltinwords( const char * word );
};

} // namespace utils
