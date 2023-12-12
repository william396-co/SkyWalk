
#ifndef __SRC_DATASERVER_HASH_H__
#define __SRC_DATASERVER_HASH_H__

#include <string>
#include <cstring>
#include <stdint.h>

#include "utils/hashfunc.h"

namespace data {

// 比较函数
struct KeyCompare
{
    bool operator()( const char * s1, const char * s2 ) const
    {
        return std::strcmp( s1, s2 ) == 0;
    }
};

// Hash函数
struct HashDjb
{
    size_t operator()( const char * str ) const
    {
        uint32_t h = 5381;

        for ( ; *str++ != '\0'; ) {
            h = ( h << 5 ) + h + ( uint32_t )( *str );
        }

        return h;
    }
};

struct KeyCompare64
{
    bool operator()( const std::string & s1, const std::string & s2 ) const
    {
        size_t len = s1.size();
        const char * ss1 = s1.data();
        const char * ss2 = s2.data();

        if ( len != s2.size() ) return false;

        for ( ; len >= 8; len -= 8 ) {
            if ( *(uint64_t *)ss1 != *(uint64_t *)ss2 ) return false;

            ss1 += 8;
            ss2 += 8;
        }

        if ( len == 7 ) {
            return ( ( *(uint64_t *)ss1 ^ *(uint64_t *)ss2 ) & 0x00ffffffffffffffUL ) == 0UL;
        }
        if ( len >= 4 ) {
            if ( *(uint32_t *)ss1 != *(uint32_t *)ss2 ) return false;

            ss1 += 4;
            ss2 += 4;
            len -= 4;
        }
        switch ( len ) {
            case 1: return *ss1 == *ss2;
            case 2: return *(uint16_t *)ss1 == *(uint16_t *)ss2;
            case 3: return *(uint16_t *)ss1 == *(uint16_t *)ss2 && ss1[2] == ss2[2];
        }

        return true;
    }
};

struct HashMurMur64
{
    size_t operator()( const std::string & key ) const
    {
        return utils::HashFunction::murmur64(
            key.c_str(), key.size(), 0xadc83b19ULL );
    }
};

} // namespace data

#endif
