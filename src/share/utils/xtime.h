#pragma once

#include <chrono>

namespace utils {

// seconds
inline time_t time()
{
    using namespace std::chrono;
    return duration_cast<seconds>( system_clock::now().time_since_epoch() ).count();
}
// milliseconds
inline time_t now()
{
    using namespace std::chrono;
    return duration_cast<milliseconds>( system_clock::now().time_since_epoch() ).count();
}
// microseconds
inline time_t current()
{
    using namespace std::chrono;
    return duration_cast<microseconds>( system_clock::now().time_since_epoch() ).count();
}

} // namespace utils
