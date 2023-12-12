
#include <cmath>
#include <cassert>
#include <algorithm>

#include "base62.h"

namespace utils {

Base62::Base62()
{}

Base62::~Base62()
{}

std::string Base62::encode( uint64_t value )
{
    std::string number;

    while ( value > 0 ) {
        uint32_t v = value % BASE;

        if ( v < 10 ) {
            number += '0' + v;
        } else if ( v < 36 ) {
            number += 'a' + v - 10;
        } else {
            number += 'A' + v - 36;
        }

        value /= BASE;
    }

    std::reverse( number.begin(), number.end() );

    return number;
}

uint64_t Base62::decode( const std::string & value )
{
    int32_t count = 0;
    uint64_t number = 0;

    assert( value.size() <= 11 && "value overflow" );

    for ( int32_t i = value.size() - 1; i >= 0; --i ) {
        int32_t num = 0;

        if ( value[i] >= '0' && value[i] <= '9' ) {
            num = value[i] - '0';
        } else if ( value[i] >= 'a' && value[i] <= 'z' ) {
            num = value[i] - 'a' + 10;
        } else if ( value[i] >= 'A' && value[i] <= 'Z' ) {
            num = value[i] - 'A' + 36;
        }

        number += ( uint64_t )( num * std::pow<int>( BASE, count++ ) );
    }

    return number;
}

} // namespace utils

#if 0

#include <iostream>

int main( int argc, char ** argv )
{
    if ( argc != 2 )
    {
        return -1;
    }

    std::string sys62 = utils::Base62().encode( atoll(argv[1]) );

    std::cout << sys62 << std::endl;
    std::cout << utils::Base62().decode( sys62 ) << std::endl;

    return 0;
}

#endif
