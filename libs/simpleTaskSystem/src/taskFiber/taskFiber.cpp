#include "precompiledHeader.h"
#include "TaskFiber.h"
#include "..\task\task.h"

NAMESPACE_STS_BEGIN

///////////////////////////////////////////////////////////
TaskFiber::TaskFiber()
	: __base::FiberBase( 0 )
{
	Reset();
	STATIC_ASSERT( sizeof( TaskFiber ) == BTL_CACHE_LINE_SIZE, "[TaskFiber]: class does not have size of cacheLine!" );
	ASSERT( IsAligned< BTL_CACHE_LINE_SIZE >( this ) );
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

		btl::this_fiber::SwitchToFiber( m_prevFiberID );
	}
}

///////////////////////////////////////////////////////////
void TaskFiber::Reset()
{
	m_taskToExecute = nullptr;
	m_taskManager = nullptr;
	m_prevFiberID = INVALID_FIBER_ID;
	m_state = TaskFiberState::Idle;
}

NAMESPACE_STS_END