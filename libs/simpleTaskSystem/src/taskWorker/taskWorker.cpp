#include "precompiledHeader.h"
#include "taskWorker.h"
#include "..\taskWorker\taskWorkersPool.h"
#include "..\taskFiber\taskFiberAllocator.h"

NAMESPACE_STS_BEGIN

#define THREAD_LOG( txt, ... ) LOG( "[WORKER_THREAD_ID: %i ]: " txt, m_context.m_poolIndex __VA_ARGS__ );

TaskWorkerThread::TaskWorkerThread( const TaskWorkerContext& context )
    : m_context( context )
    , m_shouldFinishWork( false )
	, m_hasFinishWork( false )
{
}

///////////////////////////////////////////////////////////
void TaskWorkerThread::ThreadFunction()
{
	m_thisWorkerFiberID = btl::this_fiber::ConvertThreadToFiber();
	ASSERT( m_thisWorkerFiberID != INVALID_FIBER_ID );
	m_currentFiber = m_context.m_fiberAllocator->AllocateNewTaskFiber();
	ASSERT( m_currentFiber != nullptr );
	m_currentFiber->Setup( m_thisWorkerFiberID, m_context.m_taskManager );

	THREAD_LOG( "Starting main loop." );

	MainWorkerThreadLoop();

	THREAD_LOG( "Main loop ended." );

	m_context.m_fiberAllocator->ReleaseTaskFiber( m_currentFiber );
	btl::this_fiber::ConvertFiberToThread();
	m_thisWorkerFiberID = INVALID_FIBER_ID;
}

void TaskWorkerThread::MainWorkerThreadLoop()
{
	while( true )
	{
		// Check if we have any new task to work on. If not, then wait for them.
		m_hasWorkToDoEvent.Wait();
		m_hasWorkToDoEvent.ResetEvent();

		THREAD_LOG( "Weaking up." );

		// Finish work if requested:
		if( m_shouldFinishWork )
		{
			THREAD_LOG( "Requested to finish work." );
			m_hasFinishWork = true;
			return;
		}

		// Do all the tasks:
		while( !m_shouldFinishWork )
		{
			Task* task = nullptr;

			// Check if there is any task in the queue - take the newst one, it works better for 
			// dynamic build trees( assumption is done, that newst tasks are related to each other and cache - friendly ).
			task = m_pendingTaskQueue.PopBack();

			// Local queue is empty, so try to steal task from other threads.
			if( task == nullptr )
			{
				THREAD_LOG( "Stealing task from workers." );
				task = StealTaskFromOtherWorkers();
			}
				
			if( task )
			{
				// We have the task, so run it now.
				ASSERT( m_currentFiber );
				ASSERT( m_currentFiber->GetCurrentState() == TaskFiberState::Idle );
				m_currentFiber->SetTaskToExecute( task );
				THREAD_LOG( "Switching to fiber to execute task." );
				btl::this_fiber::SwitchToFiber( m_currentFiber->GetFiberID() );
				ASSERT( m_currentFiber->GetCurrentState() == TaskFiberState::Idle );
				THREAD_LOG( "Switching back from fiber. Task is done." );
			}
			else
			{
				THREAD_LOG( "No more tasks to do, going to wait state." );
				break; // We don't have anything to do, so break and wait for job.
			}

			THREAD_LOG( "Has %i tasks now in queue...", , m_pendingTaskQueue.Size_NotThreadSafe() );
		}
	}
}

////////////////////////////////////////////////////////
Task* TaskWorkerThread::StealTaskFromOtherWorkers()
{
	Task* stealed_task = nullptr;

	uint32_t workers_count = m_context.m_workersPool->GetPoolSize();
	for( uint32_t i = 1; i < workers_count; ++i )
	{
		// Start from thread that is next to this worker in the pool.
		uint32_t index = ( i + m_context.m_poolIndex ) % workers_count;
		if( stealed_task = m_context.m_workersPool->GetWorkerAt( index )->TryToStealTask() )
			return stealed_task;
	}

	return nullptr;
}


NAMESPACE_STS_END