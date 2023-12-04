
#include "hashfunc.h"

namespace utils {

size_t HashFunction::ap( const char * key, size_t len )
{
    size_t hash = 0;

    for ( size_t i = 0; i < len; ++i ) {
        if ( ( i & 1 ) == 0 ) {
            hash ^= ( ( hash << 7 ) ^ key[i] ^ ( hash >> 3 ) );
        } else {
            hash ^= ( ~( ( hash << 11 ) ^ key[i] ^ ( hash >> 5 ) ) );
        }
    }

    return hash;
}

size_t HashFunction::elf( const char * key, size_t len )
{
    size_t hash = 0;
    uint32_t x = 0;

    for ( size_t i = 0; i < len; ++i ) {
        hash = ( hash << 4 ) + key[i];

        if ( ( x = hash & 0xF0000000L ) != 0 ) {
            hash ^= ( x >> 24 );
            hash &= ~x;
        }
    }

    return hash;
}

size_t HashFunction::djb( const std::string & key )
{
    return HashFunction::djb( key.c_str(), key.size() );
}

size_t HashFunction::djb( const char * key, size_t len )
{
    uint32_t h = 5381;

    for ( size_t i = 0; i < len; ++i ) {
        h = ( h << 5 ) + h + (uint32_t)key[i];
    }

    return h;
}

size_t HashFunction::sax( const char * key, size_t len )
{
    uint32_t h = 0;

    for ( size_t i = 0; i < len; ++i ) {
        h ^= ( h << 5 ) + ( h >> 2 ) + (uint8_t)key[i];
    }

    return h;
}

size_t HashFunction::sdbm( const char * key, size_t len )
{
    uint32_t h = 0;

    for ( size_t i = 0; i < len; ++i ) {
        h = (uint8_t)key[i] + ( h << 6 ) + ( h << 16 ) - h;
    }

    return h;
}

size_t HashFunction::bkdr( const char * key, size_t len )
{
    size_t hash = 0;
    uint32_t seed = 13131;

    for ( size_t i = 0; i < len; ++i ) {
        hash = hash * seed + key[i];
    }

    return hash;
}

size_t HashFunction::murmur( const char * key, size_t len )
{
    return HashFunction::murmur64( key, len );
}

uint32_t HashFunction::murmur32( const char * key, size_t len, uint32_t seed )
{
    const int32_t r = 24;
    const uint32_t m = 0x5bd1e995;

    uint32_t h = seed ^ len;
    const uint8_t * data = (const uint8_t *)key;

    while ( len >= 4 ) {
        uint32_t k = *(uint32_t *)data;

        k *= m;
        k ^= k >> r;
        k *= m;

        h *= m;
        h ^= k;

        data += 4;
        len -= 4;
    }

    switch ( len ) {
        case 3:
            h ^= data[2] << 16;
        case 2:
            h ^= data[1] << 8;
        case 1:
            h ^= data[0];
            h *= m;
    }

    h ^= h >> 13;
    h *= m;
    h ^= h >> 15;

    return h;
}

uint64_t HashFunction::murmur64( const char * key, size_t len, uint64_t seed )
{
    const int32_t r = 47;
    const uint64_t m = 0xc6a4a7935bd1e995;

    uint64_t h = seed ^ ( len * m );

    const uint8_t * data = (const uint8_t *)key;
    const uint8_t * end = data + ( len - ( len & 7 ) );

    while ( data != end ) {
        uint64_t k = 0;

#if ( 1 >> 1 == 0 )
        k = *( (uint64_t *)data );
#else
        k = (uint64_t)data[0];
        k |= (uint64_t)data[1] << 8;
        k |= (uint64_t)data[2] << 16;
        k |= (uint64_t)data[3] << 24;
        k |= (uint64_t)data[4] << 32;
        k |= (uint64_t)data[5] << 40;
        k |= (uint64_t)data[6] << 48;
        k |= (uint64_t)data[7] << 56;
#endif

        k *= m;
        k ^= k >> r;
        k *= m;
        h ^= k;
        h *= m;

        data += 8;
    }

    switch ( len & 7 ) {
        case 7: h ^= uint64_t( data[6] ) << 48;
        case 6: h ^= uint64_t( data[5] ) << 40;
        case 5: h ^= uint64_t( data[4] ) << 32;
        case 4: h ^= uint64_t( data[3] ) << 24;
        case 3: h ^= uint64_t( data[2] ) << 16;
        case 2: h ^= uint64_t( data[1] ) << 8;
        case 1:
            h ^= uint64_t( data[0] );
            h *= m;
    }

    h ^= h >> r;
    h *= m;
    h ^= h >> r;

    return h;
}

int32_t HashFunction::jump_consistent_hash( uint64_t key, int32_t nbuckets )
{
    // 1. 平衡性，把对象均匀地分布在所有桶中。
    // 2. 一次性，当桶的数量变化时，只需要把一些对象从旧桶移动到新桶，不需要做其它移动。
    int64_t b = -1, j = 0;

    while ( j < nbuckets ) {
        b = j;
        key = key * 2862933555777941757ULL + 1;
        j = ( b + 1 ) * ( double( 1LL << 31 ) / double( ( key >> 33 ) + 1 ) );
    }

    return b;
}

} // namespace utils
