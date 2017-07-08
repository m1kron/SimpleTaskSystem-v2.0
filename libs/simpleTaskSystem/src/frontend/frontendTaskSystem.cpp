#include "precompiledHeader.h"
#include "frontendTaskSystem.h"
#include "..\backend\backendTaskSystem.h"

NAMESPACE_STS_BEGIN

#define SYSTEM_LOG( ... ) LOG( "[FRONTEND_SYSTEM]: " __VA_ARGS__ );

// Implementation of dtor of ITaskSystem.
ITaskSystem::~ITaskSystem() {}

////////////////////////////////////////////////////////////////////////////////////////////////
FrontendTaskSystem::FrontendTaskSystem()
	: m_backend( nullptr )
{
}

////////////////////////////////////////////////////////////////////////////////////////////////
FrontendTaskSystem::~FrontendTaskSystem()
{
	ASSERT( m_backend == nullptr );
}

////////////////////////////////////////////////////////////////////////////////////////////////
uint32_t FrontendTaskSystem::GetWorkersCount() const
{
	return m_backend->GetWorkersCount();
}

////////////////////////////////////////////////////////////////////////////////////////////////
const ITaskHandle* FrontendTaskSystem::CreateNewTask( const ITaskHandle* dependant1, const ITaskHandle* dependant2, const ITaskHandle* dependant3 )
{
	return m_backend->CreateNewTask( dependant1, dependant2, dependant3 );
}

////////////////////////////////////////////////////////////////////////////////////////////////
bool FrontendTaskSystem::SubmitTask( const ITaskHandle* task_handle )
{
	return m_backend->SubmitTask( task_handle );;
}

////////////////////////////////////////////////////////////////////////////////////////////////
void FrontendTaskSystem::ReleaseTask( const ITaskHandle* task_handle )
{
	m_backend->ReleaseTask( task_handle );
}

////////////////////////////////////////////////////////////////////////////////////////////////
void FrontendTaskSystem::TryToRunOneTask()
{
	bool hasMoreWorkToDo = m_helperInstanceWorker.PerformOneExecutionStep();
	if( !hasMoreWorkToDo )
		btl::this_thread::SleepFor( 2 );
}

////////////////////////////////////////////////////////////////////////////////////////////////
bool FrontendTaskSystem::ConvertMainThreadToWorker()
{
	if( m_helperInstanceWorker.ConvertToWorkerInstance() )
	{
		SYSTEM_LOG( "Converting main thread to helper worker instance." );
		return true;
	}

	SYSTEM_LOG( "Attempt to covnert main thread to helper worker instance failed! Calling ConvertMainThreadToWorker from two different threads?" );

	return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////
void FrontendTaskSystem::ConvertWorkerToMainThread()
{
	VERIFY_SUCCESS( m_helperInstanceWorker.ConvertToNormalThread() );
	m_helperInstanceWorker.FlushAllPendingAndSuspendedTasks();
	SYSTEM_LOG( "Converted from helper worker instance back to normal thread again." );
}

////////////////////////////////////////////////////////////////////////////////////////////////
bool FrontendTaskSystem::Initialize( BackendTaskSystem* backend_system )
{
	ASSERT( backend_system );
	m_backend = backend_system;
	return m_backend->Initialize( m_helperInstanceWorker, this );;
}

////////////////////////////////////////////////////////////////////////////////////////////////
BackendTaskSystem* FrontendTaskSystem::Deinitialize()
{
	m_backend->Deinitialize( m_helperInstanceWorker );
	auto backend_ptr = m_backend;
	m_backend = nullptr;
	return backend_ptr;
}

NAMESPACE_STS_END