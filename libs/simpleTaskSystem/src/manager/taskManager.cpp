#include "precompiledHeader.h"
#include "taskManager.h"

#include "..\..\..\basicThreadingLib\include\tools\tools.h"
#include "..\..\..\basicThreadingLib\include\thread\functorThread.h"

NAMESPACE_STS_BEGIN

// Implementation of dtor of ITaskManager.
ITaskManager::~ITaskManager() {}

//////////////////////////////////////////////////////
TaskManager::~TaskManager()
{
	ASSERT( m_workerThreadsPool.GetPoolSize() == 0 );
	ASSERT( m_taskAllocator.AreAllTasksReleased() );
}

//////////////////////////////////////////////////////
bool TaskManager::Initialize()
{
	uint32_t num_cores = btl::GetLogicalCoresSize();

	// Heuristic: create num_cores - 1 working threads:
	m_workerThreadsPool.InitializePoolAndStartWorkers( num_cores - 1, this );
	return true;
}

void TaskManager::Deinitialize()
{
	m_workerThreadsPool.StopWorkersAndReleasePool();
}

////////////////////////////////////////////////////////
const ITaskHandle* TaskManager::CreateNewTask( const ITaskHandle* parent_task_handle )
{
	return CreateNewTaskImpl( parent_task_handle );
}

/////////////////////////////////////////////////////////
bool TaskManager::DispatchTask( Task* task )
{
	if( !task->IsReadyToBeExecuted() )
		return true; // Means that tasks has dependencies and cannot be dispatched now.

	// If submit task is called from one of the worker thread, add task to that thread,
	// for improving cache usage.
	TaskWorkerThread* this_thread_worker = m_workerThreadsPool.FindWorkerWithThreadID( btl::this_thread::GetThreadID() );

	if( this_thread_worker && this_thread_worker->AddTask( task ) )
		return true;

	// SubmitTask is called from other thread, so use normal task dispatching tactic:
	// try to dispach task equally among all worker threads:
	uint32_t counter = ++m_taskDispacherCounter;
	uint32_t workers_count = GetWorkersCount();
	uint32_t worker_id = counter % workers_count;

	for( uint32_t i = 0; i < workers_count; ++i )
	{
		// Try to add to every worker if selected one is full:
		TaskWorkerThread* worker = m_workerThreadsPool.GetWorkerAt( ( worker_id + i ) % workers_count );

		if( worker->AddTask( task ) )
			return true; // Finally, task has been added.
	}

	return false;
}

/////////////////////////////////////////////////////////
bool TaskManager::SubmitTask( const ITaskHandle* task_handle )
{
	auto handle = TaskHandle::AsTaskHandle( task_handle );
	ASSERT( handle );

	bool ret_val = DispatchTask( handle->GetTask() );

	// Wake up threads.
	WakeUpAllWorkers();

	return ret_val;
}

/////////////////////////////////////////////////////////
//bool TaskManager::SubmitTaskBatch( const TaskBatch& batch )
//{
//	for( const TaskHandle& handle : batch )
//	{
//		if( !DispatchTask( handle ) )
//			return false;
//	}
//
//	// Wake up threads.
//	WakeUpAllWorkers();
//
//	return true;
//}

/////////////////////////////////////////////////////////
void TaskManager::ReleaseTask( const ITaskHandle* task_handle )
{
	m_taskAllocator.ReleaseTask( task_handle );
}

/////////////////////////////////////////////////////////
bool TaskManager::AreAllTasksReleased() const
{
	return m_taskAllocator.AreAllTasksReleased();
}

/////////////////////////////////////////////////////////
void TaskManager::TryToRunOneTask()
{
	Task* stealed_task = nullptr;

	// try to steal a task from workers:
	// [NOTE]: try to balance stealing from all workers equally, not like this!!!
	uint32_t workers_count = m_workerThreadsPool.GetPoolSize();
	for( uint32_t i = 0; i < workers_count; ++i )
	{
		if( stealed_task = m_workerThreadsPool.GetWorkerAt( i )->TryToStealTask() )
		{
			ASSERT( stealed_task->IsReadyToBeExecuted() );
			break;
		}
	}

	// Execute task:
	if( stealed_task )
	{
		ASSERT( m_currentTaskFiber->GetCurrentState() == TaskFiberState::Idle );
		m_currentTaskFiber->SetTaskToExecute( stealed_task );
		btl::this_fiber::SwitchToFiber( m_currentTaskFiber->GetFiberID() );
		ASSERT( m_currentTaskFiber->GetCurrentState() == TaskFiberState::Idle );

	}
	else // Or wait, cuz there aren't any task to execute, the task that we are waiting for should be being executed by thread worker.
		btl::this_thread::SleepFor(2);
}

/////////////////////////////////////////////////////////
bool TaskManager::ConvertMainThreadToWorker()
{
	m_thisFiberID = btl::this_fiber::ConvertThreadToFiber();
	ASSERT( m_thisFiberID != INVALID_FIBER_ID );

	if( m_currentTaskFiber = m_workerThreadsPool.m_taskFiberAllocator.AllocateNewTaskFiber() )
	{
		m_currentTaskFiber->Setup( m_thisFiberID, this );
		return true;
	}

	return false;
}

/////////////////////////////////////////////////////////
void TaskManager::ConvertWorkerToMainThread()
{
	if( m_currentTaskFiber )
	{
		m_workerThreadsPool.m_taskFiberAllocator.ReleaseTaskFiber( m_currentTaskFiber );
		m_currentTaskFiber = nullptr;
	}

	VERIFY_SUCCES( btl::this_fiber::ConvertFiberToThread() );
	m_thisFiberID = INVALID_FIBER_ID;
}

/////////////////////////////////////////////////////////
int TaskManager::GetWorkersCount() const
{
	return m_workerThreadsPool.GetPoolSize();
}

/////////////////////////////////////////////////////////
void TaskManager::WakeUpAllWorkers() const
{
	uint32_t workers_count = GetWorkersCount();
	for( uint32_t worker_id = 0; worker_id < workers_count; ++worker_id )
	{
		m_workerThreadsPool.GetWorkerAt( worker_id )->WakeUp();
	}
}

/////////////////////////////////////////////////////////
const ITaskHandle* TaskManager::CreateNewTaskImpl( const ITaskHandle* parent_task_handle )
{
	const ITaskHandle* new_task_handle = m_taskAllocator.AllocateNewTask();

	if( new_task_handle == nullptr )
		return nullptr;

	if( parent_task_handle != nullptr )
		new_task_handle->AddParent( parent_task_handle );

	return new_task_handle;
}

NAMESPACE_STS_END