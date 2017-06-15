#include "..\implementation\tasking\taskFibers\TaskFiber.h"
#include <sts\tasking\Task.h>

NAMESPACE_STS_BEGIN

///////////////////////////////////////////////////////////
TaskFiber::TaskFiber()
	: __base::FiberBase( 0 )
	, m_taskToExecute( nullptr )
	, m_prevFiberID( INVALID_FIBER_ID )
	, m_state( TaskFiberState::Idle )
{
	STATIC_ASSERT( sizeof( TaskFiber ) == STS_CACHE_LINE_SIZE, "[TaskFiber]: class does not have size of cacheLine!" );
	ASSERT( IsAligned< STS_CACHE_LINE_SIZE >( this ) );
}

///////////////////////////////////////////////////////////
void TaskFiber::FiberFunction()
{
	while( true )
	{
		m_state = TaskFiberState::Running;

		ASSERT( m_taskToExecute != nullptr );
		ASSERT( m_taskManager != nullptr );
		ASSERT( m_prevFiberID != INVALID_FIBER_ID );

		m_taskToExecute->Run( m_taskManager );
		m_state = TaskFiberState::Idle;
		m_taskToExecute = nullptr;

		this_fiber::SwitchToFiber( m_prevFiberID );
	}
}

NAMESPACE_STS_END