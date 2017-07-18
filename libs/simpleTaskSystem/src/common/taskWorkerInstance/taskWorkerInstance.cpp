#include "precompiledHeader.h"
#include "taskWorkerInstance.h"
#include "taskWorkerInstanceHelper.h"
#include "..\..\backend\dispatcher\dispatcher.h"
#include "..\..\backend\taskFiber\taskFiberAllocator.h"
#include "..\..\backend\task\task.h"

NAMESPACE_STS_BEGIN
NAMESPACE_COMMON_BEGIN

#define WORKER_LOG( txt, ... )																						\
	if ( m_context.m_id == 0 )	{ LOG( "[HELPER_WORKER_INSTANCE< %i >]: " txt, m_context.m_id __VA_ARGS__ );	}	\
	else { LOG( "[WORKER_INSTANCE_ID< %i >]: " txt, m_context.m_id __VA_ARGS__ );	}								\

//////////////////////////////////////////////////////////////////////////////////
bool TaskWorkerInstance::Initalize( const TaskWorkerInstanceContext& context )
{
	ASSERT( context.m_fiberAllocator );
	ASSERT( context.m_taskSystem );
	ASSERT( context.m_dispatcher );

	m_context = context;
	if( m_currentFiber = m_context.m_fiberAllocator->AllocateNewTaskFiber() )
		return true;

	return false;
}

//////////////////////////////////////////////////////////////////////////////////
bool TaskWorkerInstance::ConvertToWorkerInstance()
{
	if( IsConvertedToWorkerInstance() )
	{
		ASSERT( false );
		return false; //< Already converted to fiber!!
	}

	if( !btl::this_fiber::IsThreadConvertedToFiber() )
	{
		btl::this_fiber::ConvertThreadToFiber(); //< Conver to fiber only if thread is not already converted ( e.g. user can also use  fibers! );
		m_convertedToFiberByThisWorkerInstance = true;
	}

	m_thisFiberID = btl::this_fiber::GetFiberID();
	SetupFiber( m_currentFiber );
	m_convertedThreadID = btl::this_thread::GetThreadID();
	m_convertedFlag.Store( 1, btl::MemoryOrder::Release );
	return true;
}

//////////////////////////////////////////////////////////////////////////////////
bool TaskWorkerInstance::ConvertToNormalThread()
{
	if( !IsConvertedToWorkerInstance() )
	{
		ASSERT( false );
		return false; //< Was not coverted to fiber!!
	}

	if ( m_convertedToFiberByThisWorkerInstance )
		btl::this_fiber::ConvertFiberToThread(); // Convert to thread only if this instance has converted this thread.

	m_thisFiberID = INVALID_FIBER_ID;
	m_convertedThreadID = INVALID_THREAD_ID;
	m_convertedFlag.Store( 0, btl::MemoryOrder::Release );
	return true;
}

//////////////////////////////////////////////////////////////////////////////////
bool TaskWorkerInstance::PerformOneExecutionStep()
{
	WORKER_LOG( "Has %i suspended task fibers and %i pending tasks.", , m_suspendedTaskFibers.GetCurrentSize(), m_pendingTaskQueue.GetCurrentSize() );

	bool isThereMoreWorkToDo = false;

	isThereMoreWorkToDo |= CheckAndExecuteSuspenedTaskFibers();

	if( backend::Task* task = TrytoGetTaskToExecute() )
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
void TaskWorkerInstance::SwitchToTaskFiber( backend::TaskFiber* fiber )
{
	ASSERT( fiber );
	TaskWorkerInstanceHelper::SetCurrentlyExecutedTask( fiber );
	btl::this_fiber::SwitchToFiber( fiber->GetFiberID() );
}

//////////////////////////////////////////////////////////////////////////////////
void TaskWorkerInstance::HandleCurrentTaskFiberSwitch()
{
	TaskWorkerInstanceHelper::SetCurrentlyExecutedTask( nullptr );

	WORKER_LOG( "Switching back from current fiber." );

	switch( m_currentFiber->GetCurrentState() )
	{
	case backend::TaskFiberState::Idle:
		OnFinishedTaskFiber( m_currentFiber );
		break;
	case backend::TaskFiberState::Suspended:
		OnSuspendedCurrrentTaskFiber();
		break;

	default:
		ASSERT( false );
	}
}

//////////////////////////////////////////////////////////////////////////////////
void TaskWorkerInstance::OnFinishedTaskFiber( backend::TaskFiber* fiber )
{
	backend::Task* finished_task = fiber->GetTask();
	WORKER_LOG( "Task< %i > is done.",, finished_task->GetTaskID() );

	// Check if task has any dependency - if has and it is ready, then submit it now.
	backend::TReadyToBeExecutedArray array { nullptr };
	if( finished_task->UpdateDependecies( m_context.m_registry, array ) )
	{
		for( backend::Task* ready_to_be_executed : array )
		{
			if( ready_to_be_executed )
			{
				WORKER_LOG( "Just-finished-task< %i > has a dependant task< %i > ready to be executed, so adding it to the local pending queue.", , finished_task->GetTaskID(), ready_to_be_executed->GetTaskID() );
				VERIFY_SUCCESS( AddTask( ready_to_be_executed ) ); //< Add task to local queue.
			}
		}
	}

	// Clear task in fiber.
	fiber->SetTaskToExecute( nullptr );
}

//////////////////////////////////////////////////////////////////////////////////
void TaskWorkerInstance::OnSuspendedCurrrentTaskFiber()
{
	WORKER_LOG( "Current task fiber with task< %i > is suspended. Add it to suspended fiber queue and get a new one.",, m_currentFiber->GetTask()->GetTaskID() );
	if( backend::TaskFiber* new_task_fiber = m_context.m_fiberAllocator->AllocateNewTaskFiber() )
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
		backend::TaskFiber* current_suspended_task_fiber = m_suspendedTaskFibers.PopFront();

		if( current_suspended_task_fiber == nullptr )
		{
			WORKER_LOG( "All suspended fiber tasks are stolen." );
			break;
		}

		WORKER_LOG( "Switching to suspended task fiber with task< %i >.",, current_suspended_task_fiber->GetTask()->GetTaskID() );
		SwitchToTaskFiber( current_suspended_task_fiber );
		if( HandleSuspendedTaskFiberSwitch( current_suspended_task_fiber ) )
			break; // We have finished a task!
	}

	return suspendedSize > 0;
}

//////////////////////////////////////////////////////////////////////////////////
bool TaskWorkerInstance::HandleSuspendedTaskFiberSwitch( backend::TaskFiber* fiber )
{
	TaskWorkerInstanceHelper::SetCurrentlyExecutedTask( nullptr );

	WORKER_LOG( "Switching back from so-far-suspended fiber with task< %i >.",, fiber->GetTask()->GetTaskID() );

	switch( fiber->GetCurrentState() )
	{
	case backend::TaskFiberState::Idle:
		WORKER_LOG( "Suspended fiber with task< %i > is done now.", , fiber->GetTask()->GetTaskID() );
		OnFinishedTaskFiber( fiber );
		m_context.m_fiberAllocator->ReleaseTaskFiber( fiber ); //< Release task fiber.
		return true;
	case backend::TaskFiberState::Suspended:
		WORKER_LOG( "Suspended fiber with task< %i > is still suspended.", , fiber->GetTask()->GetTaskID() );
		m_suspendedTaskFibers.PushBack( fiber ); //< Add this task fiber back to suspended queue.
		return false;

	default:
		ASSERT( false );
	}

	return false;
}

//////////////////////////////////////////////////////////////////////////////////
void TaskWorkerInstance::ExecuteSingleTask( backend::Task* task )
{
	WORKER_LOG( "Executing the task< %i >.",, task->GetTaskID() );
	ASSERT( m_currentFiber );
	ASSERT( m_currentFiber->GetCurrentState() == backend::TaskFiberState::Idle );
	m_currentFiber->SetTaskToExecute( task );
	WORKER_LOG( "Switching to current fiber to execute task< %i >.", , task->GetTaskID() );
	SwitchToTaskFiber( m_currentFiber );
	HandleCurrentTaskFiberSwitch();
}

//////////////////////////////////////////////////////////////////////////////////
backend::Task* TaskWorkerInstance::TrytoGetTaskToExecute()
{
	WORKER_LOG( "Trying to get new task to execute." );
	// Check if there is any task in the queue - take the newest one, it works better for 
	// dynamic build trees( assumption is done, that newst tasks are related to each other and cache - friendly ).
	backend::Task* task = m_pendingTaskQueue.PopBack();

	// Local queue is empty, so try to steal task from other threads.
	if( task == nullptr )
	{
		WORKER_LOG( "Try to steal a task from other workers." );
		task = TryToStealTaskFromOtherInstances();
	}

	return task;
}

//////////////////////////////////////////////////////////////////////////////////
backend::Task* TaskWorkerInstance::TryToStealTaskFromOtherInstances()
{
	return m_context.m_dispatcher->TryToStealTaskFromOtherWorkerInstances( GetInstanceID() );
}

//////////////////////////////////////////////////////////////////////////////////
void TaskWorkerInstance::ReleaseFiber( backend::TaskFiber* fiber )
{
	m_context.m_fiberAllocator->ReleaseTaskFiber( fiber );
}

//////////////////////////////////////////////////////////////////////////////////
void TaskWorkerInstance::SetupFiber( backend::TaskFiber* fiber )
{
	fiber->Setup( m_thisFiberID, m_context.m_taskSystem );
}

//////////////////////////////////////////////////////////////////////////////////
bool TaskWorkerInstance::AddTask( backend::Task* task )
{
	WORKER_LOG( "Added new task< %i > to local pending queue.", , task->GetTaskID() );
	return m_pendingTaskQueue.PushBack( task );
}

//////////////////////////////////////////////////////////////////////////////////
void TaskWorkerInstance::FlushAllPendingAndSuspendedTasks()
{
	WORKER_LOG( "Flushing %i suspended task fibers and %i pending tasks to other worker instances.", , m_suspendedTaskFibers.GetCurrentSize(), m_pendingTaskQueue.GetCurrentSize() );
	// 2. Flush pending tasks:
	while( backend::TaskFiber* suspended_task_fiber = m_suspendedTaskFibers.PopBack() )
		VERIFY_SUCCESS( m_context.m_dispatcher->RedispatchSuspendedTaskFiber( suspended_task_fiber ) );

	// 2. Flush pending tasks:
	while( backend::Task* pending_task = m_pendingTaskQueue.PopBack() )
		VERIFY_SUCCESS( m_context.m_dispatcher->RedispatchTaskFromHelperWorkerInstance( pending_task ) );
}

//////////////////////////////////////////////////////////////////////////////////
bool TaskWorkerInstance::TakeOwnershipOfSuspendedTaskFiber( backend::TaskFiber* suspended_task_fiber )
{
	ASSERT( suspended_task_fiber->GetCurrentState() == backend::TaskFiberState::Suspended );
	if( m_suspendedTaskFibers.PushBack( suspended_task_fiber ) )
	{
		WORKER_LOG( "Taken ownership of suspended task fiber with task< %i >", , suspended_task_fiber->GetTask()->GetTaskID() );
		suspended_task_fiber->Setup( m_thisFiberID, m_context.m_taskSystem );
		return true;
	}
	return false;
}

NAMESPACE_COMMON_END
NAMESPACE_STS_END