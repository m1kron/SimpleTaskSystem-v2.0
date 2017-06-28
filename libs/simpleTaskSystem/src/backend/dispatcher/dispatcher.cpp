#include "precompiledHeader.h"
#include "dispatcher.h"
#include "..\..\task\task.h"
#include "..\..\taskWorker\taskWorkerInstance.h"
#include "..\..\tools\positiveNumberHasher.h"
#include "..\taskFiber\taskFiber.h"

NAMESPACE_STS_BEGIN

#define DISPATCHER_LOG( ... ) //LOG( "[DISPATCHER]: " __VA_ARGS__ );

/////////////////////////////////////////////////////////////////////////////
uint32_t Dispatcher::Register( TaskWorkerInstance* instance, bool primary_instance )
{
	m_allRegisteredInstances.push_back( instance );

	if( primary_instance )
		m_primaryInstances.push_back( instance );
	else
		m_helpersInstances.push_back( instance );

	return GetRegisteredInstancesCount() - 1;
}

/////////////////////////////////////////////////////////////////////////////
void Dispatcher::UnregisterAll()
{
	m_allRegisteredInstances.clear();
	m_primaryInstances.clear();
	m_helpersInstances.clear();
}

/////////////////////////////////////////////////////////////////////////////
bool Dispatcher::DispatchTask( Task* task )
{
	const auto this_thread_id = btl::this_thread::GetThreadID();
	DISPATCHER_LOG( "Requested to dispatch task< %i > from thread id: %i.", task->GetTaskID(), this_thread_id );

	ASSERT( task->IsReadyToBeExecuted() );

	// 1) If task is submited from one of the available worker instances, add task to that instance:
	if( TaskWorkerInstance* instance = FindWorkerInstanceWithThreadID( this_thread_id ) )
	{
		if( instance->AddTask( task ) )
		{
			DISPATCHER_LOG( "DispatchTask() called from worker instance with %i, added task< %i > to that instance.", instance->GetInstanceID(), task->GetTaskID() );
			WakeUpAllPrimaryWorkerInstances();
			return true;
		}
		else
		{
			DISPATCHER_LOG( "DispatchTask() called from worker instance with %i, but dispatcher has failed to add task< %i > to that instance.", instance->GetInstanceID(), task->GetTaskID() );
		}
	}

	// 2) Task is not submitted from any of available worker instances, so dispatch task to one of them. 
	return DispatchTaskToPrimaryInstances( task );
}

/////////////////////////////////////////////////////////////////////////////
bool Dispatcher::RedispatchTaskFromHelperWorkerInstance( Task* task )
{
	DISPATCHER_LOG( "Requested to redispatch task< %i > from helper worker instance.", task->GetTaskID() );
	ASSERT( task->IsReadyToBeExecuted() );
	return DispatchTaskToPrimaryInstances( task );
}

/////////////////////////////////////////////////////////////////////////////
bool Dispatcher::RedispatchSuspendedTaskFiber( TaskFiber* suspended_task_fiber )
{
	const uint32_t taskID = suspended_task_fiber->GetTask()->GetTaskID();
	DISPATCHER_LOG( "Requested to redispatch suspended task fiber with task< %i > from helper worker instance.", suspended_task_fiber->GetTask()->GetTaskID() );

	// Try to dispach task equally among all primary worker threads. In order to do that use a hash of task id:
	const uint32_t primarySize = ( uint32_t )m_primaryInstances.size();
	uint32_t starting_id = CalcHashedNumberClamped( taskID, primarySize );

	for( uint32_t i = 0; i < primarySize; ++i )
	{
		uint32_t worker_instance_idx = ( starting_id + i ) % primarySize;
		TaskWorkerInstance* instance = m_primaryInstances[ worker_instance_idx ];
		if( instance->TakeOwnershipOfSuspendedTaskFiber( suspended_task_fiber ) )
		{
			DISPATCHER_LOG( "Suspended task fiber with task< %i > was added to worker instance %i.", taskID, instance->GetInstanceID() );
			WakeUpAllPrimaryWorkerInstances();
			return true;
		}
	}

	DISPATCHER_LOG( "Failed to dispatch suspended task fiber with task< %i >.", taskID );
	return false;
}

/////////////////////////////////////////////////////////////////////////////
bool Dispatcher::DispatchTaskToPrimaryInstances( Task* task )
{
	// Try to dispach task equally among all primary worker threads. In order to do that use a hash of task id:
	const uint32_t primarySize = ( uint32_t )m_primaryInstances.size();
	uint32_t starting_id = CalcHashedNumberClamped( task->GetTaskID(), primarySize );

	for( uint32_t i = 0; i < primarySize; ++i )
	{
		uint32_t worker_instance_idx = ( starting_id + i ) % primarySize;
		TaskWorkerInstance* instance = m_primaryInstances[ worker_instance_idx ];

		if( instance->AddTask( task ) )
		{
			DISPATCHER_LOG( "Added task< %i > to worker instance %i.", task->GetTaskID(), instance->GetInstanceID() );
			WakeUpAllPrimaryWorkerInstances();
			return true; // Finally, task has been added.
		}
	}

	DISPATCHER_LOG( "Failed to submit task< %i >.", task->GetTaskID() );
	return false;
}

/////////////////////////////////////////////////////////////////////////////
Task* Dispatcher::TryToStealTaskFromOtherWorkerInstances( uint32_t requesting_worker_instance_id )
{
	Task* stealed_task = nullptr;

	const uint32_t instances_count = GetRegisteredInstancesCount();
	for( uint32_t i = 1; i < instances_count; ++i )
	{
		// Start from thread that is next to this worker in the pool.
		uint32_t index = ( i + requesting_worker_instance_id ) % instances_count;
		TaskWorkerInstance* instance = m_allRegisteredInstances[ index ];
		if( stealed_task = instance->TryToStealTaskFromThisInstance() )
		{
			DISPATCHER_LOG( "Stealed task from worker instance( id: %i ), for worker instance( id: %i ).", instance->GetInstanceID(), requesting_worker_instance_id );
			return stealed_task;
		}
	}

	return nullptr;
}

/////////////////////////////////////////////////////////////////////////////
TaskWorkerInstance* Dispatcher::FindWorkerInstanceWithThreadID( btl::THREAD_ID thread_id )
{
	// 1. Check always converted instances:
	for( auto instance : m_primaryInstances )
		if( instance->GetThreadID() == thread_id )
			return instance;

	// 2. Chcek not alwyas converted instances:
	for( auto instance : m_helpersInstances )
		if( instance->IsConvertedToWorkerInstance() && instance->GetThreadID() == thread_id )
			return instance;

	return nullptr;
}

NAMESPACE_STS_END