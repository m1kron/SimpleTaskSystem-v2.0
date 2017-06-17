#pragma once
#include "taskWorker.h"
#include "..\taskFiber\taskFiberAllocator.h"

NAMESPACE_STS_BEGIN

class TaskManager;

////////////////////////////////////////////////////////////
// Manages pool of worker threads.
class TaskWorkersPool
{
public:
	// Initializes to have specified size and starts all workers.
	void InitializePoolAndStartWorkers( uint32_t num_of_workers, TaskManager* task_manager );

	// Send stop signal to all workers in pool and WAITS for them untill they are finished.
	// Releases then whole pool.
	void StopWorkersAndReleasePool();

	// Returns worker at given index. Returns null if there aren't such index in the pool.
	TaskWorkerThread* GetWorkerAt( uint32_t index ) const;

	// Returns worker that have specified thread_id. Returns null if such thread does not exist in the pool.
	TaskWorkerThread* FindWorkerWithThreadID( btl::THREAD_ID thread_id ) const;

	// Returns size of the pool.
	uint32_t GetPoolSize() const;

	sts::TaskFiberAllocator m_taskFiberAllocator; // < HACK!! Cannot be here!

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


NAMESPACE_STS_END