#include "precompiledHeader.h"
#include "taskWorkerInstance.h"
#include "taskWorkerInstanceHub.h"
#include "..\taskFiber\taskFiberAllocator.h"
#include "..\task\task.h"
#include "..\manager\taskManager.h"

NAMESPACE_STS_BEGIN

#define WORKER_LOG( txt, ... )																					\
	if ( m_context.m_id == 0 )	{ LOG( "[MAIN_THREAD_INSTANCE< %i >]: " txt, m_context.m_id __VA_ARGS__ );	}	\
	else { LOG( "[WORKER_INSTANCE_ID< %i >]: " txt, m_context.m_id __VA_ARGS__ );	}							\

//////////////////////////////////////////////////////////////////////////////////
bool TaskWorkerInstance::Initalize( const TaskWorkerInstanceContext& context )
{
	ASSERT( context.m_fiberAllocator );
	ASSERT( context.m_taskManager );
	ASSERT( context.m_taskWorkerInstancesHub );

	m_context = context;
	if( m_currentFiber = m_context.m_fiberAllocator->AllocateNewTaskFiber() )
		return true;

	return false;
}

//////////////////////////////////////////////////////////////////////////////////
bool TaskWorkerInstance::ConvertToFiber()
{
	ASSERT( !IsConvertedToFiber() );
	m_thisFiberID = btl::this_fiber::ConvertThreadToFiber();
	SetupFiber( m_currentFiber );
	return IsConvertedToFiber();
}

//////////////////////////////////////////////////////////////////////////////////
bool TaskWorkerInstance::ConvertToThread()
{
	ASSERT( IsConvertedToFiber() );
	btl::this_fiber::ConvertFiberToThread();
	m_thisFiberID = INVALID_FIBER_ID;
	return !IsConvertedToFiber();
}

//////////////////////////////////////////////////////////////////////////////////
bool TaskWorkerInstance::TryToExecuteSingleTask()
{
	Task* task = nullptr;

	// Check if there is any task in the queue - take the newest one, it works better for 
	// dynamic build trees( assumption is done, that newst tasks are related to each other and cache - friendly ).
	task = m_pendingTaskQueue.PopBack();

	// Local queue is empty, so try to steal task from other threads.
	if( task == nullptr )
	{
		WORKER_LOG( "Stealing task from workers." );
		task = TryToStealTaskFromOtherInstances();
	}

	if( task )
	{
		// We have the task, so run it now.
		ASSERT( m_currentFiber );
		ASSERT( m_currentFiber->GetCurrentState() == TaskFiberState::Idle );
		m_currentFiber->SetTaskToExecute( task );
		WORKER_LOG( "Switching to fiber to execute task." );
		btl::this_fiber::SwitchToFiber( m_currentFiber->GetFiberID() );
		ASSERT( m_currentFiber->GetCurrentState() == TaskFiberState::Idle );
		WORKER_LOG( "Switching back from fiber." );

		OnFinishedTaskFiber( m_currentFiber );
	}
	else
	{
		WORKER_LOG( "No more tasks to do, going to wait state." );
		return false; // We don't have anything to do, so break and wait for job.
	}

	WORKER_LOG( "Has %i tasks now in queue...",, m_pendingTaskQueue.GetCurrentSize() );

	return true;
}

//////////////////////////////////////////////////////////////////////////////////
void TaskWorkerInstance::OnFinishedTaskFiber( TaskFiber* finished_task_fiber )
{
	WORKER_LOG( "Task is done." );
	Task* finished_task = finished_task_fiber->GetTask();

	// Check if task has any dependency - if has and it is ready, then submit it now.
	if( Task* parent_of_finished_task = finished_task->GetParentTask() )
		if( parent_of_finished_task->IsReadyToBeExecuted() )
			VERIFY_SUCCESS( AddTask( parent_of_finished_task ) ); //< Add task to local queue.

	// Clear task in fiber.
	finished_task_fiber->SetTaskToExecute( nullptr );
}

//////////////////////////////////////////////////////////////////////////////////
Task* TaskWorkerInstance::TryToStealTaskFromOtherInstances()
{
	Task* stealed_task = nullptr;

	const uint32_t instances_count = m_context.m_taskWorkerInstancesHub->GetRegisteredInstancesCount();
	for( uint32_t i = 1; i < instances_count; ++i )
	{
		// Start from thread that is next to this worker in the pool.
		uint32_t index = ( i + m_context.m_id ) % instances_count;
		if( stealed_task = m_context.m_taskWorkerInstancesHub->GetRegisteredInstanceAt( index )->TryToStealTaskFromThisInstance() )
			return stealed_task;
	}

	return nullptr;
}

//////////////////////////////////////////////////////////////////////////////////
void TaskWorkerInstance::ReleaseFiber( TaskFiber* fiber )
{
	m_context.m_fiberAllocator->ReleaseTaskFiber( fiber );
}

//////////////////////////////////////////////////////////////////////////////////
void TaskWorkerInstance::SetupFiber( TaskFiber* fiber )
{
	fiber->Setup( m_thisFiberID, m_context.m_taskManager );
}

//////////////////////////////////////////////////////////////////////////////////
bool TaskWorkerInstance::AddTask( Task* task )
{
	WORKER_LOG( "Added new task to local queue." );
	return m_pendingTaskQueue.PushBack( task );
}


NAMESPACE_STS_END