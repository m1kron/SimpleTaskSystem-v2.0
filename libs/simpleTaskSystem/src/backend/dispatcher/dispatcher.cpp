#include "precompiledHeader.h"
#include "dispatcher.h"
#include "..\..\task\task.h"
#include "..\..\taskWorker\taskWorkerInstance.h"
#include "..\..\tools\positiveNumberHasher.h"

NAMESPACE_STS_BEGIN

#define DISPATCHER_LOG( ... ) LOG( "[DISPATCHER]: " __VA_ARGS__ );

/////////////////////////////////////////////////////////////////////////////
uint32_t Dispatcher::Register( TaskWorkerInstance* instance, bool always_converted )
{
	m_allRegisteredInstances.push_back( instance );

	if( always_converted )
		m_alwaysConvertedInstances.push_back( instance );
	else
		m_notAlwaysConvertedInstances.push_back( instance );

	return GetRegisteredInstancesCount() - 1;
}

/////////////////////////////////////////////////////////////////////////////
void Dispatcher::UnregisterAll()
{
	m_allRegisteredInstances.clear();
	m_alwaysConvertedInstances.clear();
	m_notAlwaysConvertedInstances.clear();
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
			return true;
		}
		else
		{
			DISPATCHER_LOG( "DispatchTask() called from worker instance with %i, but dispatcher has failed to add task< %i > to that instance.", instance->GetInstanceID(), task->GetTaskID() );
		}
	}

	// 2) Task is not submitted from any of available worker instances, so dispatch task to one of them. 
	// Try to dispach task equally among all worker threads. In order to do that use a hash of task id:
	const uint32_t alwaysConvertedSize = (uint32_t)m_alwaysConvertedInstances.size();
	uint32_t starting_id = CalcHashedNumberClamped( task->GetTaskID(), alwaysConvertedSize );

	for( uint32_t i = 0; i < alwaysConvertedSize; ++i )
	{
		uint32_t worker_instance_idx = ( starting_id + i ) % alwaysConvertedSize;
		TaskWorkerInstance* instance = m_alwaysConvertedInstances[ worker_instance_idx ];

		if( instance->AddTask( task ) )
		{
			DISPATCHER_LOG( "Added task< %i > to worker instance %i.", task->GetTaskID(), instance->GetInstanceID() );
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
	for( auto instance : m_alwaysConvertedInstances )
		if( instance->GetThreadID() == thread_id )
			return instance;

	// 2. Chcek not alwyas converted instances:
	for( auto instance : m_alwaysConvertedInstances )
		if( instance->IsConvertedToFiber() && instance->GetThreadID() == thread_id )
			return instance;

	return nullptr;
}

NAMESPACE_STS_END