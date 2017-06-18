#pragma once
#include "taskWorkerThread.h"


NAMESPACE_STS_BEGIN

////////////////////////////////////////////////////////////
// Manages pool of worker threads.
class TaskWorkersPool
{
public:
	// Initializes to have specified size.
	void InitializePool( uint32_t num_of_workers );

	// Releases whole pool.
	void ReleasePool();

	// Returns worker at given index. Returns null if there aren't such index in the pool.
	TaskWorkerThread* GetWorkerAt( uint32_t index ) const;

	// Returns worker that have specified thread_id. Returns null if such thread does not exist in the pool.
	TaskWorkerThread* FindWorkerWithThreadID( btl::THREAD_ID thread_id ) const;

	// Returns size of the pool.
	uint32_t GetPoolSize() const;

private:
	std::vector< std::unique_ptr< TaskWorkerThread > > m_workerThreads;
};

////////////////////////////////////////////////////////////////////
//
// INLINES:
//
////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////
inline TaskWorkerThread* TaskWorkersPool::GetWorkerAt( uint32_t index ) const
{
	ASSERT( index < m_workerThreads.size() );
	return m_workerThreads[ index ].get();
}

////////////////////////////////////////////////////////////////////
inline uint32_t TaskWorkersPool::GetPoolSize() const
{
	return (uint32_t)m_workerThreads.size();
}

////////////////////////////////////////////////////////////////////
inline void TaskWorkersPool::ReleasePool()
{
	// Release all worker instances;
	m_workerThreads.clear();
}


NAMESPACE_STS_END