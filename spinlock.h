#ifndef __COMMON_SPINLOCK_H
#define __COMMON_SPINLOCK_H

#include <atomic>

class SpinLock
{
public:
	SpinLock()
	{
		m_flag.clear();
	}

	SpinLock(const SpinLock&) = delete;

	SpinLock& operator=(const SpinLock&) = delete;

	void lock()
	{
		while(m_flag.test_and_set(std::memory_order_acquire));
	}
	void unlock()
	{
		m_flag.clear(std::memory_order_release);
	}

private:
	std::atomic_flag m_flag;
};

#endif
