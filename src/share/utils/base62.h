
#ifndef __SRC_UTILS_BASE62_H__
#define __SRC_UTILS_BASE62_H__

#include <string>
#include <stdint.h>

namespace utils {

class Base62
{
public:
    Base62();
    ~Base62();
    std::string encode( uint64_t value );
    uint64_t decode( const std::string & value );

private:
    enum { BASE = 62 };
};

} // namespace utils

#endif
