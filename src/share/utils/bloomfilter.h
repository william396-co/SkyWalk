
#ifndef __SRC_UTILS_BLOOMFILTER_H__
#define __SRC_UTILS_BLOOMFILTER_H__

#include <stdint.h>
#include <unistd.h>

#include "hashfunc.h"

namespace utils {

class BloomFilter
{
public:
    BloomFilter()
        : m_Count( 0 ),
          m_Bitsets( nullptr )
    {
        m_HashFuncs[0] = nullptr;
        m_HashFuncs[1] = nullptr;
    }

    ~BloomFilter()
    {
        m_Count = 0;
        m_Bitsets = nullptr;
        m_HashFuncs[0] = nullptr;
        m_HashFuncs[1] = nullptr;
    }

public:
    bool init( uint32_t count );
    void final();

    bool add( const char * key, size_t nkey = 0 );
    bool check( const char * key, size_t nkey = 0 );

public:
    static const uint32_t BitsPerWord;
    static const uint32_t BitsWordMask;

    static inline uint32_t bit_mask( uint32_t word );
    static inline uint32_t bit_word( uint32_t word );

private:
    uint32_t m_Count;
    unsigned long * m_Bitsets;

    HashFunction::func m_HashFuncs[2];
};

} // namespace utils

#endif
