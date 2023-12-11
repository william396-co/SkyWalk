
#ifndef __SRC_UTILS_XMATH_H__
#define __SRC_UTILS_XMATH_H__

#include <stdint.h>
#include <unistd.h>

namespace utils {

class Math
{
public:
    static float fabs( float f );
    static int32_t abs( int32_t x );

public:
    // 最接近幂运算
    static size_t roundpow2( size_t v );
    // 计算x的二进制数中1的个数
    static int32_t count1bits( uint32_t x );
    // 相等精度判断
    static bool equal2( double a, double b, double epsilon ) {
        double diff = a - b;
        return diff >= -epsilon && diff <= epsilon;
    }
};

struct Overflow
{
    // 向上溢出
    template<typename T>
        static bool upper( T value, T addon )
        {
            return addon > 0 && value + addon < value;
        }

    // 向下溢出
    template<typename T>
        static bool lower( T value, T addon )
        {
            return addon < 0 && value < ( T )( ~addon + 1 );
        }

    // 是否溢出
    template<typename T>
        static bool check( T value, T addon )
        {
            return upper( value, addon ) || lower( value, addon );
        }
};

} // namespace utils

#endif
