#include "precompiledHeader.h"
#include "taskWorkersPool.h"

NAMESPACE_STS_BEGIN

////////////////////////////////////////////////////////////////////
void TaskWorkersPool::InitializePoolAndStartWorkers( unsigned num_of_workers, TaskManager* task_manager )
{
	// Create requested number of thread:
	for( unsigned i = 0; i < num_of_workers; ++i )
		m_workerThreads.push_back( std::unique_ptr< TaskWorkerThread >( new TaskWorkerThread( { this, task_manager, &m_taskFiberAllocator, i } ) ) );

	// Start threads:
	for( unsigned i = 0; i < num_of_workers; ++i )
		m_workerThreads[ i ]->StartThread();
}

////////////////////////////////////////////////////////////////////
void TaskWorkersPool::StopWorkersAndReleasePool()
{
	// Signal all worker that they should finish right now.
	for( auto& worker : m_workerThreads )
		worker->FinishWork();

	// Wait until all workers are done;
	for( auto& worker : m_workerThreads )
		worker->Join();

	// Release all worker instances;
	m_workerThreads.clear();
}

////////////////////////////////////////////////////////////////////
TaskWorkerThread* TaskWorkersPool::FindWorkerWithThreadID( btl::THREAD_ID thread_id ) const
{
	for( const std::unique_ptr< TaskWorkerThread >& worker : m_workerThreads )
		if( worker->GetThreadID() == thread_id )
			return worker.get();

	return nullptr;
}

NAMESPACE_STS_END