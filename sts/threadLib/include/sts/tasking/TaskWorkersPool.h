#pragma once

#include <sts\private_headers\common\NamespaceMacros.h>
#include <sts\private_headers\common\Platform.h>
#include <sts\tasking\TaskWorker.h>
#include "..\implementation\tasking\taskFibers\taskFiberAllocator.h"
#include <vector>
#include <memory>

NAMESPACE_STS_BEGIN

class TaskManager;

////////////////////////////////////////////////////////////
// Manages pool of worker threads.
class TaskWorkersPool
{
public:
	// Initializes to have specified size and starts all workers.
	void InitializePoolAndStartWorkers( unsigned num_of_workers, TaskManager* task_manager );

	// Send stop signal to all workers in pool and WAITS for them untill they are finished.
	// Releases then whole pool.
	void StopWorkersAndReleasePool();

	// Returns worker at given index. Returns null if there aren't such index in the pool.
	TaskWorkerThread* GetWorkerAt( unsigned index ) const;

	// Returns worker that have specified thread_id. Returns null if such thread does not exist in the pool.
	TaskWorkerThread* FindWorkerWithThreadID( THREAD_ID thread_id ) const;

	// Returns size of the pool.
	unsigned GetPoolSize() const;

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
inline TaskWorkerThread* TaskWorkersPool::GetWorkerAt( unsigned index ) const
{
	ASSERT( index < m_workerThreads.size() );
	return m_workerThreads[ index ].get();
}

////////////////////////////////////////////////////////////////////
inline unsigned TaskWorkersPool::GetPoolSize() const
{
	return (unsigned)m_workerThreads.size();
}


NAMESPACE_STS_END