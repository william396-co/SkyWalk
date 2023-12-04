#pragma once

#include <map>
#include <vector>
#include <cassert>
#include <numeric>
#include <algorithm>
#include <cstdint>
#include <cstring>

#include "base/support.h"

// UnorderedMap定义
#if defined(__APPLE__) || defined(__darwin__)
    #include <unordered_set>
    #include <unordered_map>
    #define UnorderedSet std::unordered_set
    #define UnorderedMap std::unordered_map
#else
    #if ( __GNUC__ < 4 || __GNUC__ == 4 && __GNUC_MINOR__ < 1 )
        #include <set>
        #include <map>
        #define UnorderedSet std::set
        #define UnorderedMap std::map
    #else
        #if USE_CPP11
            #include <unordered_set>
            #include <unordered_map>
            #define UnorderedSet std::unordered_set
            #define UnorderedMap std::unordered_map
        #else
            #include <tr1/unordered_set>
            #include <tr1/unordered_map>
            #define UnorderedSet std::tr1::unordered_set
            #define UnorderedMap std::tr1::unordered_map
        #endif
    #endif
#endif

// 分支预测
#define likely(x)       __builtin_expect( (x), 1 )
#define unlikely(x)     __builtin_expect( (x), 0 )

#define SAFE_DELETE(x)  { if (x) { delete (x); (x) = nullptr; } }


// ASSERT
#define ASSERT( cond, message, expr ) \
    assert( (cond) && (message) ); if ( !(cond) ) { expr }

// 点分十进制IP
typedef std::vector<uint16_t>   IpAddress;

// 全局变量
namespace global
{
    // 全局时间修正
    extern int32_t delta_timestamp;
}

struct pair_hash {
    template<class T1, class T2>
        std::size_t operator()( const std::pair<T1, T2> & p ) const
        {
            auto h1 = std::hash<T1> {}( p.first );
            auto h2 = std::hash<T2> {}( p.second );
            return h1 ^ h2;
        }
};

template<typename T, typename F, size_t... S>
void foreach_tuple_impl( T&& tuple, F&& func, std::index_sequence<S...> ) {
    std::initializer_list<int>{ ( func( std::get<S>( tuple ) ), 0 )... };
}

template<typename T, typename F> void foreach_tuple( T&& tuple, F&& func ) {
    // c++ 14 的 make_index_sequence
    foreach_tuple_impl(
        std::forward<T>( tuple ), std::forward<F>( func ),
        std::make_index_sequence<std::tuple_size<std::decay_t<T>>::value>{} );
}

struct Sourcefile
{
    Sourcefile() : data(nullptr), size(0), line(0) {}

    template<size_t N>
        Sourcefile( const char (&arr)[N], size_t l )
            : data( arr ),
              size( N-1 ),
              line( l )
    {
        const char * slash = strrchr( data, '/' );
        if ( slash ) {
            data = slash + 1;
            size = data - arr;
        }
    }

    Sourcefile( const char * file, size_t l )
        : data( file ),
          size( 0 ),
          line( l )
    {
        const char * slash = strrchr( file, '/' );
        if ( slash ) {
            data = slash + 1;
        }

        size = strlen( data );
    }

    bool empty() const
    {
        return (data == nullptr || size == 0 || line == 0);
    }

    const char * data;
    size_t size;
    size_t line;
};
