
#ifndef __SRC_UTILS_HASHFUNC_H__
#define __SRC_UTILS_HASHFUNC_H__

#include <string>
#include <stdint.h>
#include <unistd.h>

namespace utils {

class HashFunction
{
public:
    typedef size_t ( *func )( const char *, size_t );

    // djb, sax, sdbm 分布均为32位无符号整数
    static size_t djb( const std::string & key );
    static size_t djb( const char * key, size_t len );
    static size_t sax( const char * key, size_t len );
    static size_t sdbm( const char * key, size_t len );
    //
    static size_t ap( const char * key, size_t len );
    static size_t elf( const char * key, size_t len );
    static size_t bkdr( const char * key, size_t len );
    static size_t murmur( const char * key, size_t len );

public:
    static uint32_t murmur32( const char * key, size_t len, uint32_t seed = 0x19851209 );
    static uint64_t murmur64( const char * key, size_t len, uint64_t seed = 0x19851209 );

public:
    // A Fast, Minimal Memory, Consistent Hash Algorithm
    // https://arxiv.org/ftp/arxiv/papers/1406/1406.2294.pdf
    static int32_t jump_consistent_hash( uint64_t key, int32_t nbuckets );
};

} // namespace utils

#endif
