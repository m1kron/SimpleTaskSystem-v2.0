#pragma once
#include "task.h"
#include "taskHandle.h"
#include "..\structures\lockfree\lockfreePtrQueue.h"

NAMESPACE_STS_BEGIN

// Lock free task allocator: preallocates pool of tasks.
class TaskAllocator
{
public:
	// Not thread safe ctor.
	TaskAllocator();
	~TaskAllocator();

	// Allocates new task, this is lock free method.
	const ITaskHandle* AllocateNewTask();

	// Release task back to pool, lock free method.
	void ReleaseTask( const ITaskHandle* task );

	// Releases all tasks.
	void ReleaseAllTasks();

	// Returns true if all tasks are released.
	bool AreAllTasksReleased() const;

	// Returns size of task pool.
	static uint32_t GetTaskPoolSize();

private:
	Task m_taskPool[ TASK_POOL_SIZE ];
	TaskHandle m_taskHandlePool[ TASK_POOL_SIZE ];
	LockFreePtrQueue< const ITaskHandle, TASK_POOL_SIZE > m_freelist;
};

NAMESPACE_STS_END