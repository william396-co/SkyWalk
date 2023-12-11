
#ifndef __SRC_UTILS_CRC_H__
#define __SRC_UTILS_CRC_H__

#include <string>
#include <stdint.h>

namespace utils {

//
// CRC-16/XMODEM
// 多项式: 16+x12+x5+1
// 初始值: 0
//
// 测试网址: http://www.ip33.com/crc.html
//
class CRC16
{
public:
    CRC16();
    ~CRC16();
    uint16_t calc( const std::string & src );
    uint16_t calc( const char * src, uint32_t len );

private:
    static uint16_t m_CrcTable[256];
};

//
// CRC32
// 多项式: 32+x26+x23+x22+x16+x12+x11+x10+x8+x7+x5+x4+x2+x+1
// 初始值: 0xFFFFFFFF
//
class CRC32
{
public:
    CRC32();
    ~CRC32();
    uint32_t calc( const std::string & src );
    uint32_t calc( const char * src, uint32_t len );

private:
    uint32_t m_CrcTable[256];
};

} // namespace utils

#endif
