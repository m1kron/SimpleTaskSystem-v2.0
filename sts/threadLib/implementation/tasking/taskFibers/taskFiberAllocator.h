#pragma once
#include <sts\private_headers\common\NamespaceMacros.h>
#include "..\implementation\tasking\taskFibers\taskFiber.h"
#include <sts\structures\lockfree\lockfreePtrQueue.h>

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
	TaskFiber m_fiberPool[ 512 ];									//< main pool of fibers of static size ( for now ).
	LockFreePtrQueue< TaskFiber, TASK_FIBER_POOL_SIZE > m_freeList;
};

NAMESPACE_STS_END