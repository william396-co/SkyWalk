
#include <cassert>
#include <cstring>

#include "bloomfilter.h"

namespace utils {

const uint32_t BloomFilter::BitsPerWord = sizeof( unsigned long ) << 3;
const uint32_t BloomFilter::BitsWordMask = BloomFilter::BitsPerWord - 1;

uint32_t BloomFilter::bit_mask( uint32_t word )
{
    return 1 << ( word & BitsPerWord );
}

uint32_t BloomFilter::bit_word( uint32_t word )
{
    return word / BitsPerWord;
}

bool BloomFilter::init( uint32_t count )
{
    uint32_t size = ( count + BitsWordMask ) / BitsPerWord;
    unsigned long * sets = new unsigned long[size];

    assert( sets != nullptr );
    assert( ( count & ( count - 1 ) ) == 0 );

    m_Count = count;
    m_Bitsets = sets;
    m_HashFuncs[0] = HashFunction::sax;
    m_HashFuncs[1] = HashFunction::djb;

    return true;
}

void BloomFilter::final()
{
    if ( m_Bitsets ) {
        delete[] m_Bitsets;
        m_Bitsets = nullptr;
    }
}

bool BloomFilter::add( const char * key, size_t nkey )
{
    uint32_t word = 0;
    uint32_t hashindex = 0;

    nkey = ( nkey == 0 ? std::strlen( key ) : nkey );

    for ( uint32_t i = 0; i < 2; ++i ) {
        hashindex = m_HashFuncs[i]( key, nkey );
        word = hashindex & ( m_Count - 1 );

        m_Bitsets[bit_word( word )] |= bit_mask( word );
    }

    return true;
}

bool BloomFilter::check( const char * key, size_t nkey )
{
    uint32_t word = 0;
    uint32_t hashindex = 0;

    nkey = ( nkey == 0 ? std::strlen( key ) : nkey );

    for ( uint32_t i = 0; i < 2; ++i ) {
        hashindex = m_HashFuncs[i]( key, nkey );
        word = hashindex & ( m_Count - 1 );

        if ( !( m_Bitsets[bit_word( word )] & bit_mask( word ) ) ) {
            return false;
        }
    }

    return true;
}

} // namespace utils
