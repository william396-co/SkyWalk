#pragma once
#include <string>
#include <cxxabi.h>

namespace utils {


inline std::string demangle( const std::string & name )
{
    int status = -4;
    std::unique_ptr<char, void ( * )( void * )> res {
        abi::__cxa_demangle( name.c_str(), nullptr, nullptr, &status ), std::free
    };
    return status == 0 ? res.get() : name;
}
} // namespace utils
