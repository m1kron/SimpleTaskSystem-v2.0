#include <sts\tasking\TaskManager.h>
#include <sts\tools\Tools.h>
#include <sts\tasking\Task.h>
#include <sts\lowlevel\thread\FunctorThread.h>
#include <sts\tasking\TaskBatch.h>

NAMESPACE_STS_BEGIN

//////////////////////////////////////////////////////
TaskManager::~TaskManager()
{
	unsigned workers_count = GetWorkersCount();

	// Signal all worker that they should finish right now.
	for( unsigned worker_id = 0; worker_id < workers_count; ++worker_id )
	{
		m_workerThreadsPool.GetWorkerAt( worker_id )->FinishWork();
	}

	// We have to wait for threads to finish their work.
	unsigned worker_id = 0;
	while( worker_id < workers_count )
	{
		// Check if thread has finish it's work.
		if( m_workerThreadsPool.GetWorkerAt( worker_id )->HasFinishedWork() )
		{
			// Go to next thread.
			++worker_id;
		}
		else
		{
			// Yield exection to give worker threads processor time.
			sts::this_thread::YieldThread();
		}
	}

	m_workerThreadsPool.ReleasePool();
	ASSERT( m_taskAllocator.AreAllTasksReleased() );
}

//////////////////////////////////////////////////////
void TaskManager::Setup()
{
	unsigned num_cores = tools::GetLogicalCoresSize();

	// Heuristic: create num_cores - 1 working threads:
	m_workerThreadsPool.InitializePool( num_cores - 1, this );;
}

////////////////////////////////////////////////////////
TaskHandle TaskManager::CreateNewTask( Task::TFunctionPtr task_function, const TaskHandle& parent_task_handle )
{
	TaskHandle new_task_handle = CreateNewTaskImpl( parent_task_handle );

	if( new_task_handle != INVALID_TASK_HANDLE )
		new_task_handle->SetTaskFunction( task_function );

	return new_task_handle;
}

/////////////////////////////////////////////////////////
bool TaskManager::DispatchTask( const TaskHandle& task_handle )
{
	if( !task_handle->IsReadyToBeExecuted() )
		return true; // Means that tasks has dependencies and cannot be dispatched now.

	// If submit task is called from one of the worker thread, add task to that thread,
	// for improving cache usage.
	TaskWorkerThread* this_thread_worker = m_workerThreadsPool.FindWorkerWithThreadID( this_thread::GetThreadID() );

	if( this_thread_worker && this_thread_worker->AddTask( task_handle.m_task ) )
		return true;

	// SubmitTask is called from other thread, so use normal task dispatching tactic:
	// try to dispach task equally among all worker threads:
	unsigned counter = ++m_taskDispacherCounter;
	unsigned workers_count = GetWorkersCount();
	unsigned worker_id = counter % workers_count;

	for( unsigned i = 0; i < workers_count; ++i )
	{
		// Try to add to every worker if selected one is full:
		TaskWorkerThread* worker = m_workerThreadsPool.GetWorkerAt( ( worker_id + i ) % workers_count );

		if( worker->AddTask( task_handle.m_task ) )
			return true; // Finally, task has been added.
	}

	return false;
}

/////////////////////////////////////////////////////////
bool TaskManager::SubmitTask( const TaskHandle& task_handle )
{
	bool ret_val = DispatchTask( task_handle );

	// Wake up threads.
	WakeUpAllWorkers();

	return ret_val;
}

/////////////////////////////////////////////////////////
bool TaskManager::SubmitTaskBatch( const TaskBatch& batch )
{
	for( const TaskHandle& handle : batch )
	{
		if( !DispatchTask( handle ) )
			return false;
	}

	// Wake up threads.
	WakeUpAllWorkers();

	return true;
}

/////////////////////////////////////////////////////////
void TaskManager::ReleaseTask( TaskHandle& task_handle )
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
	unsigned workers_count = m_workerThreadsPool.GetPoolSize();
	for( unsigned i = 0; i < workers_count; ++i )
	{
		if( stealed_task = m_workerThreadsPool.GetWorkerAt( i )->TryToStealTask() )
		{
			ASSERT( stealed_task->IsReadyToBeExecuted() );
			break;
		}
	}

	// Execute task:
	if( stealed_task )
		stealed_task->Run( this );
	else // Or wait, cuz there aren't any task to execute, the task that we are waiting for should be being executed by thread worker.
		sts::this_thread::YieldThread();
}

/////////////////////////////////////////////////////////
void TaskManager::WakeUpAllWorkers() const
{
	unsigned workers_count = GetWorkersCount();
	for( unsigned worker_id = 0; worker_id < workers_count; ++worker_id )
	{
		m_workerThreadsPool.GetWorkerAt( worker_id )->WakeUp();
	}
}

/////////////////////////////////////////////////////////
TaskHandle TaskManager::CreateNewTaskImpl( const TaskHandle& parent_task_handle )
{
	TaskHandle new_task_handle = m_taskAllocator.AllocateNewTask();

	if( new_task_handle == INVALID_TASK_HANDLE )
		return INVALID_TASK_HANDLE;

	if( parent_task_handle != INVALID_TASK_HANDLE )
		new_task_handle->AddParent( parent_task_handle );

	return new_task_handle;
}

NAMESPACE_STS_END