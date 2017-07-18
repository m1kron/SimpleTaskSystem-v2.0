#include "precompiledHeader.h"
#include "taskWorkerThreadPool.h"

NAMESPACE_STS_BEGIN
NAMESPACE_BACKEND_BEGIN

////////////////////////////////////////////////////////////////////
void TaskWorkersPool::InitializePool( uint32_t num_of_workers )
{
	// Create requested number of thread:
	for( uint32_t i = 0; i < num_of_workers; ++i )
		m_workerThreads.push_back( std::unique_ptr< TaskWorkerThread >( new TaskWorkerThread() ) );
}

////////////////////////////////////////////////////////////////////
TaskWorkerThread* TaskWorkersPool::FindWorkerWithThreadID( btl::THREAD_ID thread_id ) const
{
	for( const std::unique_ptr< TaskWorkerThread >& worker : m_workerThreads )
		if( worker->GetThreadID() == thread_id )
			return worker.get();

	return nullptr;
}

NAMESPACE_BACKEND_END
NAMESPACE_STS_END