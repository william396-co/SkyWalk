
#ifndef __SRC_UTILS_LZ4_H__
#define __SRC_UTILS_LZ4_H__

#include <string>
#include <stdint.h>

namespace utils {

class LZ4
{
public:
    // 获取压缩后的长度
    static size_t getCompressBound( size_t len );

    // 压缩
    static bool compress(
        const std::string & src, std::string & dst );

    // 压缩
    static bool compress(
        const std::string & src, void * dst, size_t & dstlen );

    // 解压
    static bool uncompress( const std::string & src, std::string & dst );
};

} // namespace utils

#endif
