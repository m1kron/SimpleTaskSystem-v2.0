#include "precompiledHeader.h"
#include "taskFiberAllocator.h"

NAMESPACE_STS_BEGIN
NAMESPACE_BACKEND_BEGIN

/////////////////////////////////////////////////////////
TaskFiberAllocator::TaskFiberAllocator()
{
	// Add all fibers to free list.
	for( uint32_t i = 0; i < common::TASK_FIBER_POOL_SIZE; ++i )
	{
		VERIFY_SUCCESS( m_freeList.PushBack( &m_fiberPool[ i ] ) );
	}
}

NAMESPACE_BACKEND_END
NAMESPACE_STS_END