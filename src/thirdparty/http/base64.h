
#ifndef __SRC_HTTP_BASE64_H__
#define __SRC_HTTP_BASE64_H__

#include <string>

class Base64
{
public:
    // 编码
    static void encode( const std::string & src, std::string & dst );
    // 解码
    static void decode( const std::string & src, std::string & dst );
};

#endif
