#include "precompiledHeader.h"
#include "taskAllocator.h"

NAMESPACE_STS_BEGIN

///////////////////////////////////////////////////
TaskAllocator::TaskAllocator()
{
	// Setup and add to free list.
	for( uint32_t i = 0; i < TASK_POOL_SIZE; ++i )
	{
		Task& task = m_taskPool[ i ];
		task.Setup( (TASK_ID)i );
		m_taskHandlePool[ i ].AssociateTask( &task );
		VERIFY_SUCCESS( m_freelist.PushBack( &m_taskHandlePool[ i ] ) );
	}
}

///////////////////////////////////////////////////
TaskAllocator::~TaskAllocator()
{
	ASSERT( AreAllTasksReleased() );
}

///////////////////////////////////////////////////
const ITaskHandle* TaskAllocator::AllocateNewTask()
{
	return m_freelist.PopFront();
}

////////////////////////////////////////////////////
void TaskAllocator::ReleaseTask( const ITaskHandle* task_handle )
{
	ASSERT( task_handle != nullptr );
	const TaskHandle* handle = TaskHandle::AsTaskHandle( task_handle );

	// Make task available to others.
	handle->GetTask()->Clear();

	// Add to free list.
	VERIFY_SUCCESS( m_freelist.PushBack( handle ) );
}

////////////////////////////////////////////////////
bool TaskAllocator::AreAllTasksReleased() const
{
	return m_freelist.GetCurrentSize() == m_freelist.GetMaxSize();;
}

////////////////////////////////////////////////////
Task* TaskAllocator::TaskIDToTask( TASK_ID id )
{
	ASSERT( id != INVALID_TASK_ID );
	return &m_taskPool[ id ];
}

////////////////////////////////////////////////////
uint32_t TaskAllocator::GetTaskPoolSize()
{
	return TASK_POOL_SIZE;
}

NAMESPACE_STS_END