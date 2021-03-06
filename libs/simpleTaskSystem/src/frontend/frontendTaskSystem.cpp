#include "precompiledHeader.h"
#include "frontendTaskSystem.h"
#include "..\backend\backendTaskSystem.h"
#include "..\common\taskWorkerInstance\taskWorkerInstanceHelper.h"

NAMESPACE_STS_BEGIN

// Implementation of dtor of ITaskSystem.
ITaskSystem::~ITaskSystem() {}

NAMESPACE_FRONTEND_BEGIN

#define SYSTEM_LOG( ... ) LOG( "[FRONTEND_SYSTEM]: " __VA_ARGS__ );

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
const ITaskHandle* FrontendTaskSystem::CreateNewTask()
{
	return m_backend->CreateNewTask();
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
void FrontendTaskSystem::WaitOnConvertedMainThread()
{
	bool hasMoreWorkToDo = m_helperInstanceWorker.PerformOneExecutionStep();
	if( !hasMoreWorkToDo )
		btl::this_thread::SleepFor( 2 );
}

////////////////////////////////////////////////////////////////////////////////////////////////
bool FrontendTaskSystem::ConvertMainThreadToWorkerInstance()
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
void FrontendTaskSystem::ConvertWorkerInstanceToMainThread()
{
	VERIFY_SUCCESS( m_helperInstanceWorker.ConvertToNormalThread() );
	m_helperInstanceWorker.FlushAllPendingAndSuspendedTasks();
	SYSTEM_LOG( "Converted from helper worker instance back to normal thread again." );
}

////////////////////////////////////////////////////////////////////////////////////////////////
bool FrontendTaskSystem::IsOnWorkerInstance() const
{
	return common::TaskWorkerInstanceHelper::IsOnActiveTaskWorkerInstance();
}

////////////////////////////////////////////////////////////////////////////////////////////////
void FrontendTaskSystem::WaitOnWorkerInstance() const
{
	common::TaskWorkerInstanceHelper::SuspendExecutionOfCurrentTask();
}

////////////////////////////////////////////////////////////////////////////////////////////////
bool FrontendTaskSystem::Initialize( backend::BackendTaskSystem* backend_system )
{
	ASSERT( backend_system );
	m_backend = backend_system;
	return m_backend->Initialize( m_helperInstanceWorker, this );
}

////////////////////////////////////////////////////////////////////////////////////////////////
backend::BackendTaskSystem* FrontendTaskSystem::Deinitialize()
{
	m_backend->Deinitialize( m_helperInstanceWorker );
	auto backend_ptr = m_backend;
	m_backend = nullptr;
	return backend_ptr;
}

NAMESPACE_BACKEND_END
NAMESPACE_STS_END