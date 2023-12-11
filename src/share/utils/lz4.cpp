
#include <cstdlib>
#include <stdint.h>

#include "lz4.h"
#include "detail/lz4/lz4.h"

//
// LZ4解压时必须知道未压缩前的长度
// 鉴于这个原因，必须在数据压缩后加上长度前缀
// LZ4最大压缩长度是int32_t, 所以前缀的长度是4个字节
// 根据LZ4的压缩比最大为255, 所以本模块的前缀使用的是压缩比
// 长度只需要1个字节
//

namespace utils {

size_t LZ4::getCompressBound( size_t len )
{
    return LZ4_compressBound( len ) + sizeof( uint8_t );
}

bool LZ4::compress( const std::string & src, void * dst, size_t & dstlen )
{
    // 计算压缩后的长度
    size_t len = LZ4::getCompressBound( src.size() );

    // 目标缓冲区是否能放下数据
    if ( len > dstlen ) {
        return false;
    }

    // 压缩
    dstlen = LZ4_compress(
        (const char *)src.data(), (char *)dst + sizeof( uint8_t ), src.size() );
    if ( dstlen <= 0 ) {
        return false;
    }

    // 写入压缩比
    *( (uint8_t *)dst ) = src.size() / dstlen;

    return true;
}

bool LZ4::compress( const std::string & src, std::string & dst )
{
    int32_t len = src.size();

    // 计算压缩后的长度
    len = LZ4_compressBound( len );
    len += sizeof( uint8_t );

    // 分配空间
    dst.resize( len );

    // 压缩
    const char * input = (const char *)src.data();
    char * output = (char *)dst.data() + sizeof( uint8_t );
    len = LZ4_compress( input, output, src.size() );
    if ( len < 0 ) {
        return false;
    }

    // 写入压缩比
    *(uint8_t *)( dst.data() ) = src.size() / len;
    dst.resize( len + sizeof( uint8_t ) );

    return true;
}

bool LZ4::uncompress( const std::string & src, std::string & dst )
{
    if ( src.size() < sizeof( uint8_t ) ) {
        return false;
    }

    const char * input = (const char *)src.data();

    // 计算得到压缩比
    uint8_t radio = *(uint8_t *)input;
    int32_t compressed_len = src.size() - sizeof( uint8_t );
    int32_t uncompress_len = ( radio + 1 ) * compressed_len;

    // 分配空间
    dst.resize( uncompress_len + 1 );

    // 解压缩
    input += sizeof( uint8_t );
    char * output = (char *)dst.data();
    int32_t len = LZ4_uncompress_unknownOutputSize(
        input, output, compressed_len, uncompress_len );
    if ( len < 0 ) {
        return false;
    }

    // 重置长度
    dst.resize( len );
    return true;
}

} // namespace utils
