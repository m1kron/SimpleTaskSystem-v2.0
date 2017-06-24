#include "precompiledHeader.h"
#include "taskWorkerInstance.h"
#include "taskWorkerInstanceHub.h"
#include "..\taskFiber\taskFiberAllocator.h"
#include "..\task\task.h"
#include "..\manager\taskManager.h"

NAMESPACE_STS_BEGIN

#define WORKER_LOG( txt, ... )																					\
	if ( m_context.m_id == 0 )	{ LOG( "[MAIN_WORKER_INSTANCE< %i >]: " txt, m_context.m_id __VA_ARGS__ );	}	\
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
	WORKER_LOG( "Has %i suspended task fibers and %i pending tasks.", , m_suspendedTaskFibers.GetCurrentSize(), m_pendingTaskQueue.GetCurrentSize() );

	bool isThereMoreWorkToDo = false;

	isThereMoreWorkToDo |= CheckAndExecuteSuspenedTaskFibers();

	if( Task* task = TrytoGetTaskToExecute() )
	{
		// We have the task, so run it now.
		ExecuteSingleTask( task );
		isThereMoreWorkToDo = true;
	}

	// We don't have anything to do, so break and wait for job.
	if ( !isThereMoreWorkToDo )
		WORKER_LOG( "Nothing to do." );

	return isThereMoreWorkToDo;
}

//////////////////////////////////////////////////////////////////////////////////
void TaskWorkerInstance::SwitchToTaskFiber( TaskFiber* fiber )
{
	ASSERT( fiber );
	btl::this_fiber::SwitchToFiber( fiber->GetFiberID() );
}

//////////////////////////////////////////////////////////////////////////////////
void TaskWorkerInstance::HandleCurrentTaskFiberSwitch()
{
	WORKER_LOG( "Switching back from current fiber." );

	switch( m_currentFiber->GetCurrentState() )
	{
	case TaskFiberState::Idle:
		OnFinishedTaskFiber( m_currentFiber );
		break;
	case TaskFiberState::Suspended:
		OnSuspendedCurrrentTaskFiber();
		break;

	default:
		ASSERT( false );
	}
}

//////////////////////////////////////////////////////////////////////////////////
void TaskWorkerInstance::OnFinishedTaskFiber( TaskFiber* fiber )
{
	WORKER_LOG( "Task is done." );
	Task* finished_task = fiber->GetTask();

	// Check if task has any dependency - if has and it is ready, then submit it now.
	if( Task* parent_of_finished_task = finished_task->GetParentTask() )
	{
		WORKER_LOG( "Just-finished-task to the local queue." );
		if( parent_of_finished_task->IsReadyToBeExecuted() )
		{
			WORKER_LOG( "Just-finished-task has a parent task ready to be executed, so adding it to the local pending queue." );
			VERIFY_SUCCESS( AddTask( parent_of_finished_task ) ); //< Add task to local queue.
		}
		else
		{
			WORKER_LOG( "Just-finished-task has a parent task, but it cannot be executed now." );
		}
	}
	else
	{
		WORKER_LOG( "Task does not have parent task." );
	}

	// Clear task in fiber.
	fiber->SetTaskToExecute( nullptr );
}

//////////////////////////////////////////////////////////////////////////////////
void TaskWorkerInstance::OnSuspendedCurrrentTaskFiber()
{
	WORKER_LOG( "Current task fiber is suspended. Add it to suspended fiber queue and get a new one." );
	if( TaskFiber* new_task_fiber = m_context.m_fiberAllocator->AllocateNewTaskFiber() )
	{
		SetupFiber( new_task_fiber );
		VERIFY_SUCCESS( m_suspendedTaskFibers.PushBack( m_currentFiber ) );
		m_currentFiber = new_task_fiber;
	}
	else
	{
		ASSERT( false );
		WORKER_LOG( "Couldn't get new task fiber!" );
		// TODO: What to do is such case? Probably the best idea will be to keep trying to executed already
		// suspended task fibers, unitl one is finally done...
	}
}

//////////////////////////////////////////////////////////////////////////////////
bool TaskWorkerInstance::CheckAndExecuteSuspenedTaskFibers()
{
	WORKER_LOG( "Checking suspended task fibers." );
	// [NOTE]: We have to check status of all suspended fibers. While doing that, this worker instance 
	// can get a new suspended task fiber, so we have to take that into account.
	// Additionally, we don't want to spin here until one task is finish, we want to make up to one pass
	// over suspended fibers and give up if all are still suspended - it is better to execute next task if it is posssible.
	// We have to stop if any of so-far-suspened task fibers is done.

	const uint32_t suspendedSize = m_suspendedTaskFibers.GetCurrentSize();
	for ( uint32_t i = 0; i < suspendedSize; ++i )
	{
		TaskFiber* current_suspended_task_fiber = m_suspendedTaskFibers.PopFront();

		if( current_suspended_task_fiber == nullptr )
		{
			WORKER_LOG( "All suspended fiber tasks are stolen." );
			break;
		}

		WORKER_LOG( "Switching to suspended task fiber." );
		SwitchToTaskFiber( current_suspended_task_fiber );
		if( HandleSuspendedTaskFiberSwitch( current_suspended_task_fiber ) )
			break; // We have finished a task!
	}

	return suspendedSize > 0;
}

//////////////////////////////////////////////////////////////////////////////////
bool TaskWorkerInstance::HandleSuspendedTaskFiberSwitch( TaskFiber* fiber )
{
	WORKER_LOG( "Switching back from so-far-suspended fiber." );

	switch( fiber->GetCurrentState() )
	{
	case TaskFiberState::Idle:
		WORKER_LOG( "Suspended fiber is done now." );
		OnFinishedTaskFiber( fiber );
		m_context.m_fiberAllocator->ReleaseTaskFiber( fiber ); //< Release task fiber.
		return true;
	case TaskFiberState::Suspended:
		WORKER_LOG( "Suspended fiber is still suspended." );
		m_suspendedTaskFibers.PushBack( fiber ); //< Add this task fiber back to suspended queue.
		return false;

	default:
		ASSERT( false );
	}

	return false;
}

//////////////////////////////////////////////////////////////////////////////////
void TaskWorkerInstance::ExecuteSingleTask( Task* task )
{
	WORKER_LOG( "Executing the task." );
	ASSERT( m_currentFiber );
	ASSERT( m_currentFiber->GetCurrentState() == TaskFiberState::Idle );
	m_currentFiber->SetTaskToExecute( task );
	WORKER_LOG( "Switching to current fiber to execute task." );
	SwitchToTaskFiber( m_currentFiber );
	HandleCurrentTaskFiberSwitch();
}

//////////////////////////////////////////////////////////////////////////////////
Task* TaskWorkerInstance::TrytoGetTaskToExecute()
{
	WORKER_LOG( "Trying to get new task to execute." );
	// Check if there is any task in the queue - take the newest one, it works better for 
	// dynamic build trees( assumption is done, that newst tasks are related to each other and cache - friendly ).
	Task* task = m_pendingTaskQueue.PopBack();

	// Local queue is empty, so try to steal task from other threads.
	if( task == nullptr )
	{
		WORKER_LOG( "Try to steal a task from other workers." );
		task = TryToStealTaskFromOtherInstances();
	}

	return task;
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
		{
			WORKER_LOG( "Stealed task from worker instance with index %i",, index );
			return stealed_task;
		}
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
	WORKER_LOG( "Added new task to local pending queue." );
	return m_pendingTaskQueue.PushBack( task );
}


NAMESPACE_STS_END