#include "precompiledHeader.h"
#include "backendTaskSystem.h"

#include "..\..\..\basicThreadingLib\include\tools\tools.h"

NAMESPACE_STS_BEGIN

#define SYSTEM_LOG( ... ) LOG( "[BACKEND_SYSTEM]: " __VA_ARGS__ );

///////////////////////////////////////////////////////////////////////////////////
bool BackendTaskSystem::Initialize( TaskWorkerInstance& helper_instance, ITaskSystem* system_interface )
{
	ASSERT( system_interface );
	const uint32_t num_cores = btl::GetLogicalCoresSize() - 1;

	SYSTEM_LOG( "Initalizing with %i worker threads.", num_cores );

	m_dispatcher.SetWakeUpAllPrimaryWorkersFunction( [ this ]() { WakeUpAllWorkerThreads(); } );

	// Init main worker instance.
	auto idx = m_dispatcher.Register( &helper_instance, false );
	VERIFY_SUCCESS( helper_instance.Initalize( { system_interface, &m_dispatcher, &m_taskFiberAllocator, idx } ) );

	// Heuristic: create num_cores - 1 working threads:
	m_workerThreadsPool.InitializePool( num_cores );

	// Initialize worker threads and worker instances:
	for( uint32_t i = 0; i < m_workerThreadsPool.GetPoolSize(); ++i )
	{
		TaskWorkerThread* workerThread = m_workerThreadsPool.GetWorkerAt( i );
		
		auto idx = m_dispatcher.Register( workerThread->GetWorkerInstance(), true );
		VERIFY_SUCCESS( workerThread->Start( { system_interface, &m_dispatcher, &m_taskFiberAllocator, idx } ) );
	}
	
	return true;
}

///////////////////////////////////////////////////////////////////////////////////
void BackendTaskSystem::Deinitialize( TaskWorkerInstance& helper_instance )
{
	// Signal all worker that they should finish right now.
	for( uint32_t i = 0; i < m_workerThreadsPool.GetPoolSize(); ++i )
		m_workerThreadsPool.GetWorkerAt( i )->FinishWork();

	// Wait until all workers are done;
	for( uint32_t i = 0; i < m_workerThreadsPool.GetPoolSize(); ++i )
		m_workerThreadsPool.GetWorkerAt( i )->Join();

	helper_instance.Deinitalize();

	m_dispatcher.UnregisterAll();
	m_workerThreadsPool.ReleasePool();
	SYSTEM_LOG( "System deinitialized." );
}

///////////////////////////////////////////////////////////////////////////////////
bool BackendTaskSystem::SubmitTask( const ITaskHandle* task_handle )
{
	auto handle = TaskHandle::AsTaskHandle( task_handle );
	ASSERT( handle );

	auto task = handle->GetTask();
	if( task->IsReadyToBeExecuted() )
		return m_dispatcher.DispatchTask( task );
	else
		SYSTEM_LOG( "Task< %i > is not ready to be executed now, will be added and executed later.", task->GetTaskID() );
	
	return true;
}

///////////////////////////////////////////////////////////////////////////////////
void BackendTaskSystem::WakeUpAllWorkerThreads() const
{
	uint32_t workers_count = GetWorkersCount();
	for( uint32_t worker_id = 0; worker_id < workers_count; ++worker_id )
		m_workerThreadsPool.GetWorkerAt( worker_id )->WakeUp();
}

///////////////////////////////////////////////////////////////////////////////////
const ITaskHandle* BackendTaskSystem::CreateNewTask( const ITaskHandle* parent_task_handle )
{
	const ITaskHandle* new_task_handle = m_taskAllocator.AllocateNewTask();

	if( new_task_handle == nullptr )
		return nullptr;

	if( parent_task_handle != nullptr )
		new_task_handle->AddParent( parent_task_handle );

	return new_task_handle;
}

NAMESPACE_STS_END