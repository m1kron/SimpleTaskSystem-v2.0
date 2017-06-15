#include<sts\tasking\TaskWorker.h>
#include<sts\tasking\Task.h>
#include<sts\private_headers\common\NamespaceMacros.h>
#include<sts\tasking\TaskWorkersPool.h>
#include "..\implementation\tasking\taskFibers\taskFiberAllocator.h"

NAMESPACE_STS_BEGIN

TaskWorkerThread::TaskWorkerThread( const TaskWorkerContext& context )
    : m_context( context )
    , m_shouldFinishWork( false )
	, m_hasFinishWork( false )
{
}

///////////////////////////////////////////////////////////
void TaskWorkerThread::ThreadFunction()
{
	m_thisWorkerFiberID = sts::this_fiber::ConvertThreadToFiber();
	m_currentFiber = m_context.m_fiberAllocator->AllocateNewTaskFiber();
	ASSERT( m_currentFiber != nullptr );
	m_currentFiber->Setup( m_thisWorkerFiberID, m_context.m_taskManager );

	MainWorkerThreadLoop();

	m_context.m_fiberAllocator->ReleaseTaskFiber( m_currentFiber );
	sts::this_fiber::ConvertFiberToThread();
	m_thisWorkerFiberID = INVALID_FIBER_ID;
}

void TaskWorkerThread::MainWorkerThreadLoop()
{
	while( true )
	{
		// Check if we have any new task to work on. If not, then wait for them.
		m_hasWorkToDoEvent.Wait();
		m_hasWorkToDoEvent.ResetEvent();

		// Finish work if requested:
		if( m_shouldFinishWork )
		{
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
				task = StealTaskFromOtherWorkers();

			if( task )
			{
				// We have the task, so run it now.
				ASSERT( m_currentFiber );
				ASSERT( m_currentFiber->GetCurrentState() == TaskFiberState::Idle );
				m_currentFiber->SetTaskToExecute( task );
				this_fiber::SwitchToFiber( m_currentFiber->GetFiberID() );
				ASSERT( m_currentFiber->GetCurrentState() == TaskFiberState::Idle );
			}
			else
				break; // We don't have anything to do, so break and wait for job.
		}
	}
}

////////////////////////////////////////////////////////
Task* TaskWorkerThread::StealTaskFromOtherWorkers()
{
	Task* stealed_task = nullptr;

	unsigned workers_count = m_context.m_workersPool->GetPoolSize();
	for( unsigned i = 1; i < workers_count; ++i )
	{
		// Start from thread that is next to this worker in the pool.
		unsigned index = ( i + m_context.m_poolIndex ) % workers_count;
		if( stealed_task = m_context.m_workersPool->GetWorkerAt( index )->TryToStealTask() )
			return stealed_task;
	}

	return nullptr;
}


NAMESPACE_STS_END