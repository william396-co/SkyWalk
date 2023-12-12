
#ifndef __SRC_HTTP_URL_H__
#define __SRC_HTTP_URL_H__

#include <string>
#include <stdint.h>

class Url
{
public :
    static void encode( const std::string & src, std::string & dst );
    static void decode( const std::string & src, std::string & dst );
};

#endif
