#pragma once
#include <sts\private_headers\common\NamespaceMacros.h>
#include <sts\tasking\Task.h>
#include <sts\tasking\TaskHandle.h>
#include <sts\tasking\TaskingCommon.h>

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
	static unsigned GetTaskPoolSize();

	// Debug stuff:
	bool Debug_TryToReleaseTask( unsigned index );
	bool Debug_IsTaskOccupied( const TaskHandle& handle );

private:
	Task m_taskPool[ TASK_POOL_SIZE ];

	// Pool markers marks whether corresponding task slot is used.
	// Could be bitfield, but it would rise probability of false sharing. 
	Atomic< unsigned > m_poolMarkers[ TASK_POOL_SIZE ];

	// Hasher is used to decrease contention.
	unsigned m_hashedNumber;
};

NAMESPACE_STS_END