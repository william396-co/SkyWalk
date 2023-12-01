#pragma once

#include <thread>
#include <atomic>

class SpinkLock
{
public:
	SpinkLock() :m_lock{} {}
	~SpinkLock() = default;

	void lock()
	{
		while (m_lock.test_and_set(std::memory_order_acquire));

	}
	void unlock() {
		m_lock.clear(std::memory_order_release);
	}
private:
	std::atomic_flag m_lock;
};
