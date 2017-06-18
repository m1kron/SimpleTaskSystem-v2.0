#pragma once
#include "taskFiber.h"
#include "..\structures\lockfree\lockfreePtrQueue.h"

NAMESPACE_STS_BEGIN

// Allocator for task fibers. TaskFibers are preallocated and added to the freelist.
// 'Allocating' a task fiber means taking next free one from the freelist.
// 'Releasing' a task fiber means adding it to the freelist.
class TaskFiberAllocator
{
public:
	TaskFiberAllocator();
	~TaskFiberAllocator();

	// Tries to allocate new task fiber. Returns nullptr if NOT succesfull.
	TaskFiber* AllocateNewTaskFiber();

	// Releases TaskFiber.
	void ReleaseTaskFiber( TaskFiber* fiber );

private:
	static const int TASK_FIBER_POOL_SIZE = 512;
	TaskFiber m_fiberPool[ TASK_FIBER_POOL_SIZE ];		//< Main pool of fibers of static size ( for now ).
	LockFreePtrQueue< TaskFiber, TASK_FIBER_POOL_SIZE > m_freeList;
};

//////////////////////////////////////////////////////////////////////////////
//
// INLINES:
//
//////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////
inline TaskFiberAllocator::~TaskFiberAllocator()
{
	ASSERT( m_freeList.GetCurrentSize() == TASK_FIBER_POOL_SIZE );
}

/////////////////////////////////////////////////////////
inline TaskFiber* TaskFiberAllocator::AllocateNewTaskFiber()
{
	return m_freeList.PopFront();
}

/////////////////////////////////////////////////////////
inline void TaskFiberAllocator::ReleaseTaskFiber( TaskFiber* fiber )
{
	fiber->Reset();
	VERIFY_SUCCESS( m_freeList.PushBack( fiber ) );
}

NAMESPACE_STS_END