#include "precompiledHeader.h"
#include "taskManager.h"

#include "..\..\..\basicThreadingLib\include\tools\tools.h"

NAMESPACE_STS_BEGIN

#define MANAGER_LOG( ... ) LOG( "[MANAGER]: " __VA_ARGS__ );

// Implementation of dtor of ITaskManager.
ITaskManager::~ITaskManager() {}

//////////////////////////////////////////////////////
bool TaskManager::Initialize()
{
	uint32_t num_cores = btl::GetLogicalCoresSize() - 1;

	MANAGER_LOG( "Initalizing with %i worker threads.", num_cores );

	// Init main thread worker instance.
	auto idx = m_workerInstancesHub.Register( &m_mainThreadInstanceWorker );
	VERIFY_SUCCESS( m_mainThreadInstanceWorker.Initalize( { this, &m_workerInstancesHub, &m_taskFiberAllocator, idx } ) );

	// Heuristic: create num_cores - 1 working threads:
	m_workerThreadsPool.InitializePool( num_cores );

	// Initialize worker threads and worker instances:
	for( uint32_t i = 0; i < m_workerThreadsPool.GetPoolSize(); ++i )
	{
		TaskWorkerThread* workerThread = m_workerThreadsPool.GetWorkerAt( i );
		
		auto idx = m_workerInstancesHub.Register( workerThread->GetWorkerInstance() );
		VERIFY_SUCCESS( workerThread->Start( { this, &m_workerInstancesHub, &m_taskFiberAllocator, idx } ) );
	}
	
	return true;
}

void TaskManager::Deinitialize()
{
	// Signal all worker that they should finish right now.
	for( uint32_t i = 0; i < m_workerThreadsPool.GetPoolSize(); ++i )
		m_workerThreadsPool.GetWorkerAt( i )->FinishWork();

	// Wait until all workers are done;
	for( uint32_t i = 0; i < m_workerThreadsPool.GetPoolSize(); ++i )
		m_workerThreadsPool.GetWorkerAt( i )->Join();

	m_mainThreadInstanceWorker.Deinitalize();

	m_workerInstancesHub.UnregisterAll();
	m_workerThreadsPool.ReleasePool();
	MANAGER_LOG( "Manager deinitialized." );
}

/////////////////////////////////////////////////////////
bool TaskManager::DispatchTask( Task* task )
{
	if( !task->IsReadyToBeExecuted() )
		return true; // Means that tasks has dependencies and cannot be dispatched now.

	// If submit task is called from one of the worker thread, add task to that thread,
	// for improving cache usage.
	TaskWorkerThread* this_thread_worker = m_workerThreadsPool.FindWorkerWithThreadID( btl::this_thread::GetThreadID() );

	if( this_thread_worker && this_thread_worker->GetWorkerInstance()->AddTask( task ) )
	{
		MANAGER_LOG( "SubmitTask called from worker thread, adding task to that thread." );
		return true;
	}

	// SubmitTask is called from other thread and main thread is acting now as a worker thread, so add task to main worker instance:
	if( m_isActingAsTaskWorker.Load( btl::MemoryOrder::Acquire ) == 1 )
	{
		MANAGER_LOG( "SubmitTask called from other thread then worker thread, but main task is acting as a worker instance, so task is added the main thread." );
		m_mainThreadInstanceWorker.AddTask( task );
		return true;
	}

	// SubmitTask is called from other thread, and main thread is not acting as a worker now, so use normal task dispatching tactic:
	// Try to dispach task equally among all worker threads:
	uint32_t counter = ++m_taskDispacherCounter;
	uint32_t workers_count = GetWorkersCount();
	uint32_t worker_id = counter % workers_count;

	for( uint32_t i = 0; i < workers_count; ++i )
	{
		uint32_t thread_worker_idx = ( worker_id + i ) % workers_count;
		// Try to add to every worker if selected one is full:
		TaskWorkerThread* worker = m_workerThreadsPool.GetWorkerAt( thread_worker_idx );

		if( worker->GetWorkerInstance()->AddTask( task ) )
		{
			MANAGER_LOG( "SubmitTask called from other thread, adding task to %i worker thread.", thread_worker_idx );
			return true; // Finally, task has been added.
		}
	}

	MANAGER_LOG( "Failed to submit task." );
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
void TaskManager::TryToRunOneTask()
{
	bool executed_anything = m_mainThreadInstanceWorker.TryToExecuteSingleTask();
	if( !executed_anything )
		btl::this_thread::SleepFor( 2 );
}

/////////////////////////////////////////////////////////
bool TaskManager::ConvertMainThreadToWorker()
{
	if( m_isActingAsTaskWorker.Exchange( 1 ) == 0 )
	{
		if( m_mainThreadInstanceWorker.ConvertToFiber() )
		{
			MANAGER_LOG( "Converting main thread to worker thread." );
			return true;
		}
	}
	else
	{
		MANAGER_LOG( "Attempt to covnert main thread to worker thread while it is already done! Calling ConvertMainThreadToWorker from two different threads?." );
	}

	MANAGER_LOG( "Failed to convert to fiber." );
	return false;
}

/////////////////////////////////////////////////////////
void TaskManager::ConvertWorkerToMainThread()
{
	VERIFY_SUCCESS( m_mainThreadInstanceWorker.ConvertToThread() );
	MANAGER_LOG( "Converting worker thread back to main thread." );
	m_isActingAsTaskWorker.Store( 0, btl::MemoryOrder::Release );
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