#include <sts\tasking\TaskWorkersPool.h>

NAMESPACE_STS_BEGIN

////////////////////////////////////////////////////////////////////
void TaskWorkersPool::InitializePool( unsigned num_of_workers, TaskManager* task_manager )
{
	// Create requested number of thread:
	for( unsigned i = 0; i < num_of_workers; ++i )
		m_workerThreads.push_back( std::unique_ptr< TaskWorkerThread >( new TaskWorkerThread( { this, task_manager, &m_taskFiberAllocator, i } ) ) );

	// Start and detach threads:
	for( unsigned i = 0; i < num_of_workers; ++i )
	{
		m_workerThreads[ i ]->StartThread();
		m_workerThreads[ i ]->Detach();
	}
}

////////////////////////////////////////////////////////////////////
TaskWorkerThread* TaskWorkersPool::FindWorkerWithThreadID( THREAD_ID thread_id ) const
{
	for( const std::unique_ptr< TaskWorkerThread >& worker : m_workerThreads )
		if( worker->GetThreadID() == thread_id )
			return worker.get();

	return nullptr;
}

NAMESPACE_STS_END