#include "precompiledHeader.h"
#include "taskFiberAllocator.h"

NAMESPACE_STS_BEGIN

/////////////////////////////////////////////////////////
TaskFiberAllocator::TaskFiberAllocator()
{
	// Add all fibers to free list.
	for( int i = 0; i < TASK_FIBER_POOL_SIZE; ++i )
	{
		VERIFY_SUCCES( m_freeList.PushBack( &m_fiberPool[ i ] ) );
	}
}

/////////////////////////////////////////////////////////
TaskFiberAllocator::~TaskFiberAllocator()
{
	ASSERT( m_freeList.Size_NotThreadSafe() == TASK_FIBER_POOL_SIZE );
}

/////////////////////////////////////////////////////////
TaskFiber* TaskFiberAllocator::AllocateNewTaskFiber()
{
	return m_freeList.PopFront();
}

/////////////////////////////////////////////////////////
void TaskFiberAllocator::ReleaseTaskFiber( TaskFiber* fiber )
{
	fiber->Reset();
	VERIFY_SUCCES( m_freeList.PushBack( fiber ) );
}

NAMESPACE_STS_END