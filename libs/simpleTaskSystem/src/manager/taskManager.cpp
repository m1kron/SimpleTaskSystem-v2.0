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
	auto idx = m_dispatcher.Register( &m_mainThreadInstanceWorker, false );
	VERIFY_SUCCESS( m_mainThreadInstanceWorker.Initalize( { this, &m_dispatcher, &m_taskFiberAllocator, idx } ) );

	// Heuristic: create num_cores - 1 working threads:
	m_workerThreadsPool.InitializePool( num_cores );

	// Initialize worker threads and worker instances:
	for( uint32_t i = 0; i < m_workerThreadsPool.GetPoolSize(); ++i )
	{
		TaskWorkerThread* workerThread = m_workerThreadsPool.GetWorkerAt( i );
		
		auto idx = m_dispatcher.Register( workerThread->GetWorkerInstance(), true );
		VERIFY_SUCCESS( workerThread->Start( { this, &m_dispatcher, &m_taskFiberAllocator, idx } ) );
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

	m_dispatcher.UnregisterAll();
	m_workerThreadsPool.ReleasePool();
	MANAGER_LOG( "Manager deinitialized." );
}

/////////////////////////////////////////////////////////
bool TaskManager::SubmitTask( const ITaskHandle* task_handle )
{
	auto handle = TaskHandle::AsTaskHandle( task_handle );
	ASSERT( handle );

	auto task = handle->GetTask();

	if( task->IsReadyToBeExecuted() )
	{
		if( m_dispatcher.DispatchTask( task ) )
			WakeUpAllWorkers();
		else
			return false; // < task is ready to be executed, but dispatcher has failed to dispatch it.
	}
	else
	{
		MANAGER_LOG( "Task< %i > is not ready to be executed now, will be added and executed later.", task->GetTaskID() );
	}
	
	return true;
}

/////////////////////////////////////////////////////////
void TaskManager::TryToRunOneTask()
{
	bool hasMoreWorkToDo = m_mainThreadInstanceWorker.TryToExecuteSingleTask();
	if( !hasMoreWorkToDo )
		btl::this_thread::SleepFor( 2 );
}

/////////////////////////////////////////////////////////
bool TaskManager::ConvertMainThreadToWorker()
{
	if( m_mainThreadInstanceWorker.ConvertToFiber() )
	{
		MANAGER_LOG( "Converting main thread to worker thread." );
		return true;
	}

	MANAGER_LOG( "Attempt to covnert main thread to worker thread Failed! Calling ConvertMainThreadToWorker from two different threads?" );

	return false;
}

/////////////////////////////////////////////////////////
void TaskManager::ConvertWorkerToMainThread()
{
	VERIFY_SUCCESS( m_mainThreadInstanceWorker.ConvertToThread() );
	MANAGER_LOG( "Converting worker thread back to main thread." );
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