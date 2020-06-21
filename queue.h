#ifndef __COMMON_QUEUE_H
#define __COMMON_QUEUE_H

#include <common/MemAlloc.h>
#include <common/types.h>

template <typename T, int pageSize = 256>
class Queue
{
public:
	Queue() = default;

	Queue(const Queue<T> &other) = delete;

	Queue& operator=(const Queue &other) = delete;
	
	void push(const T &data)
	{
		ScopedSpinLock lock(m_lock);

		QueueNode *pNode = m_poolData.Alloc(data);

		if(m_pTailNode)
		{
			m_pTailNode->pNextNode = pNode;
		}
		else
		{
			m_pHeadNode = pNode;
		}
		m_pTailNode = pNode;
	}

	void emplace(T &&data)
	{
		ScopedSpinLock lock(m_lock);

		QueueNode *pNode = m_poolData.Alloc(std::forward<T>(data));

		if(m_pTailNode)
		{
			m_pTailNode->pNextNode = pNode;
		}
		else
		{
			m_pHeadNode = pNode;
		}
		m_pTailNode = pNode;
	}
	
	bool pop(T *pOut)
	{
		ScopedSpinLock lock(m_lock);

		QueueNode *pNode = m_pHeadNode;

		if(pNode)
		{
			*pOut = std::move(pNode->data);

			m_pHeadNode = pNode->pNextNode;
			if(!m_pHeadNode)
			{
				assert(m_pTailNode == pNode);
				m_pTailNode = NULL;
			}

			m_poolData.Delete(pNode);

			return(true);
		}
		return(false);
	}

private:

	struct QueueNode
	{
		T data;
		QueueNode *pNextNode = NULL;

		QueueNode(const T &other)
		{
			data = other;
		}
		QueueNode(T &&other)
		{
			data = other;
		}
	};

	SpinLock m_lock;

	MemAlloc<QueueNode, pageSize, 16> m_poolData;

	QueueNode* m_pHeadNode = NULL;
	QueueNode* m_pTailNode = NULL;
};

#endif
