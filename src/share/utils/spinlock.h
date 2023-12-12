#pragma once

#include <thread>
#include <atomic>

namespace utils {
class SpinLock
{
public:
    SpinLock()
        : m_lock {} {}
    ~SpinLock() = default;

    void lock()
    {
        while ( m_lock.test_and_set( std::memory_order_acquire ) )
            ;
    }
    void unlock()
    {
        m_lock.clear( std::memory_order_release );
    }

private:
    std::atomic_flag m_lock;
};
} // namespace utils
