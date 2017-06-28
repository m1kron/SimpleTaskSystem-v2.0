#include "precompiledHeader.h"
#include "TaskFiber.h"
#include "..\task\task.h"
#include "..\task\taskContext.h"

NAMESPACE_STS_BEGIN

///////////////////////////////////////////////////////////
TaskFiber::TaskFiber()
	: __base::FiberBase( TASK_FIBER_STACK_SIZE )
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
		ASSERT( m_systemInterface != nullptr );
		ASSERT( m_parentFiberID != INVALID_FIBER_ID );

		TaskContext context( m_systemInterface, this );

		m_taskToExecute->Run( &context );
		m_state = TaskFiberState::Idle;
		SwitchToParentFiber();
	}
}

///////////////////////////////////////////////////////////
void TaskFiber::Reset()
{
	m_taskToExecute = nullptr;
	m_systemInterface = nullptr;
	m_parentFiberID = INVALID_FIBER_ID;
	m_state = TaskFiberState::Idle;
}

///////////////////////////////////////////////////////////
void TaskFiber::SuspendExecution()
{
	m_state = TaskFiberState::Suspended;
	SwitchToParentFiber();
}

NAMESPACE_STS_END