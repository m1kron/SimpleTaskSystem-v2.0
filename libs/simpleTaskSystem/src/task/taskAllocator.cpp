#include "precompiledHeader.h"
#include "taskAllocator.h"
#include "..\tools\positiveNumberHasher.h"

NAMESPACE_STS_BEGIN

///////////////////////////////////////////////////
TaskAllocator::TaskAllocator()
{
	// Setup and add to free list.
	for( uint32_t i = 0; i < TASK_POOL_SIZE; ++i )
	{
		m_taskHandlePool[ i ].AssociateTask( &m_taskPool[ i ] );
		VERIFY_SUCCES( m_freelist.PushBack( &m_taskHandlePool[ i ] ) );
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
	VERIFY_SUCCES( m_freelist.PushBack( handle ) );
}

////////////////////////////////////////////////////
void TaskAllocator::ReleaseAllTasks()
{
	for( uint32_t i = 0; i < TASK_POOL_SIZE; ++i )
		m_taskPool[ i ].Clear();
}

////////////////////////////////////////////////////
bool TaskAllocator::AreAllTasksReleased() const
{
	return m_freelist.GetCurrentSize() == m_freelist.GetMaxSize();;
}

////////////////////////////////////////////////////
uint32_t TaskAllocator::GetTaskPoolSize()
{
	return TASK_POOL_SIZE;
}

NAMESPACE_STS_END