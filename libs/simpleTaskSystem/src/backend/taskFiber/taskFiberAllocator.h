#pragma once
#include "taskFiber.h"
#include "..\..\common\structures\lockfree\lockfreePtrQueue.h"

NAMESPACE_STS_BEGIN
NAMESPACE_BACKEND_BEGIN

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
	TaskFiber m_fiberPool[ common::TASK_FIBER_POOL_SIZE ];		//< Main pool of fibers of static size ( for now ).
	common::LockFreePtrQueue< TaskFiber, common::TASK_FIBER_POOL_SIZE > m_freeList;
};

//////////////////////////////////////////////////////////////////////////////
//
// INLINES:
//
//////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////
inline TaskFiberAllocator::~TaskFiberAllocator()
{
	ASSERT( m_freeList.GetCurrentSize() == common::TASK_FIBER_POOL_SIZE );
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

NAMESPACE_BACKEND_END
NAMESPACE_STS_END