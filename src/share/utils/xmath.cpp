
#include "xmath.h"

namespace utils {

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wstrict-aliasing"
#pragma GCC diagnostic ignored "-Wuninitialized"

float Math::fabs( float f )
{
    int32_t tmp = *( reinterpret_cast<int32_t *>( &f ) );
    tmp &= 0x7FFFFFFF;
    return *( reinterpret_cast<float *>( &tmp ) );
}

#pragma GCC diagnostic pop

int32_t Math::abs( int32_t x )
{
    int32_t y = x >> 31;
    return ( ( x ^ y ) - y );
}

size_t Math::roundpow2( size_t v )
{
    // 超过32位有BUG，暂时注释掉
    //float f = (float)( v - 1 );
    //return (size_t)1 << ((*(size_t*)(&f) >> 23) - 126);

    // 传统方案
    --v;
    v |= v >> 1;
    v |= v >> 2;
    v |= v >> 4;
    v |= v >> 8;
    v |= v >> 16;
    v |= v >> 32;
    return ++v;
}

int32_t Math::count1bits( uint32_t x )
{
    uint32_t c = x;

    c = ( c & 0x55555555 ) + ( ( c >> 1 ) & 0x55555555 );
    c = ( c & 0x33333333 ) + ( ( c >> 2 ) & 0x33333333 );
    c = ( c & 0x0f0f0f0f ) + ( ( c >> 4 ) & 0x0f0f0f0f );
    c = ( c & 0x00ff00ff ) + ( ( c >> 8 ) & 0x00ff00ff );
    c = ( c & 0x0000ffff ) + ( ( c >> 16 ) & 0x0000ffff );

    return c;
#if 0
    int32_t c = 0;

    while ( x )
    {
        ++c;
        x = x & (x - 1);
    }

    return c;
#endif
}

} // namespace utils
