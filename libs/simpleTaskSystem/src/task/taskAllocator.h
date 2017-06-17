#pragma once
#include "task.h"
#include "taskHandle.h"

NAMESPACE_STS_BEGIN

// Lock free task allocator: preallocates pool of tasks.
class TaskAllocator
{
public:
	// Not thread safe ctor.
	TaskAllocator();

	// Allocates new task, this is lock free method.
	TaskHandle AllocateNewTask();

	// Release task back to pool, lock free method.
	void ReleaseTask( TaskHandle& task );

	// Releases all tasks.
	void ReleaseAllTasks();

	// Returns true if all tasks are released.
	bool AreAllTasksReleased() const;

	// Returns size of task pool.
	static uint32_t GetTaskPoolSize();

	// Debug stuff:
	bool Debug_TryToReleaseTask( uint32_t index );
	bool Debug_IsTaskOccupied( const TaskHandle& handle );

private:
	Task m_taskPool[ TASK_POOL_SIZE ];

	// Pool markers marks whether corresponding task slot is used.
	// Could be bitfield, but it would rise probability of false sharing. 
	btl::Atomic< uint32_t > m_poolMarkers[ TASK_POOL_SIZE ];

	// Hasher is used to decrease contention.
	uint32_t m_hashedNumber;
};

NAMESPACE_STS_END