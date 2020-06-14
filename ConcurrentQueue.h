#ifndef __ConcurrentQueue_H
#define __ConcurrentQueue_H

#include <mutex>
#include <condition_variable>
#include <queue>
#include <thread>

template<typename T> class CConcurrentQueue
{
public:
	CConcurrentQueue() = default;

	CConcurrentQueue(const CConcurrentQueue<T> &other)
	{
		m_queue = other.m_queue;
	}

	CConcurrentQueue(CConcurrentQueue<T> &&other)
	{
		m_queue.swap(other.m_queue);
	}

	bool empty() const
	{
		ScopedLock lock(m_mutex);
		return(m_queue.empty());
	}

	bool tryPop(T &out)
	{
		ScopedLock lock(m_mutex);

		if(!m_queue.empty())
		{
			out = m_queue.front();
			m_queue.pop();

			return(true);
		}

		return(false);
	}

	T pop()
	{
		ScopedLock lock(m_mutex);

		while(m_queue.empty())
		{
			m_condVar.wait(lock);
		}

		T res = m_queue.front();
		m_queue.pop();

		return(res);
	}

	void push(const T &value)
	{
		{
			ScopedLock lock(m_mutex);

			m_queue.push(value);
		}

		m_condVar.notify_one();
	}

	void emplace(T &&value)
	{
		{
			ScopedLock lock(m_mutex);

			m_queue.emplace(value);
		}

		m_condVar.notify_one();
	}

	std::size_t size() const
	{
		ScopedLock lock(m_mutex);

		return(m_queue.size());
	}

	void clear()
	{
		ScopedLock lock(m_mutex);
		std::queue<T>().swap(m_queue);
	}


	CConcurrentQueue<T> &operator=(const CConcurrentQueue<T> &other)
	{
		if(&other != this)
		{
			m_queue = other.m_queue;
		}
		return(*this);
	}

	CConcurrentQueue<T> &operator=(CConcurrentQueue<T> &&other)
	{
		if(&other != this)
		{
			m_queue.swap(other.m_queue);
		}
		return(*this);
	}

private:
	std::queue<T> m_queue;
	mutable std::mutex m_mutex;
	std::condition_variable m_condVar;
};

#endif
