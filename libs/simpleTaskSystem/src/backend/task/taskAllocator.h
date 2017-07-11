#pragma once
#include "task.h"
#include "taskHandle.h"
#include "..\..\common\structures\lockfree\lockfreePtrQueue.h"

NAMESPACE_STS_BEGIN

class TaskAllocator;

// Keeps information about allocated tasks. In particular, it allows to 
// obtain task instance from TASK ID. This is part of TaskAllocator.
class TaskRegistry
{
public:
	// Returns Task instance from TASK_ID.
	Task* TaskIDToTask( TASK_ID id ) const;

private:
	friend class TaskAllocator;
	TaskRegistry( TaskAllocator* task_allocator );

	TaskAllocator* m_taskAllocatorRef;
};

// Lock free task allocator. Task are actually preallocated, as well as
// TaskHandles. There is 1:1 mapping between task handle and task instance.
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

	// Returns true if all tasks are released.
	bool AreAllTasksReleased() const;

	// Returns task registry instance.
	const TaskRegistry* GetTaskRegistryInstance() const;

	// Returns size of task pool.
	static uint32_t GetTaskPoolSize();

private:
	friend class TaskRegistry;

	// Maps id to Task instance.
	Task* TaskIDToTask( TASK_ID id );

	// ---------------------------------------------------

	Task m_taskPool[ TASK_POOL_SIZE ];
	TaskHandle m_taskHandlePool[ TASK_POOL_SIZE ];
	LockFreePtrQueue< const ITaskHandle, TASK_POOL_SIZE > m_freelist;
	TaskRegistry m_registry;
};

////////////////////////////////////////////////////////////////////////////
//
// INLINES:
//
////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////
// TaskRegistry:
////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////
inline TaskRegistry::TaskRegistry( TaskAllocator* task_allocator )
	: m_taskAllocatorRef( task_allocator )
{
}

////////////////////////////////////////////////////////////////////////////
inline Task* TaskRegistry::TaskIDToTask( TASK_ID id ) const
{
	return m_taskAllocatorRef->TaskIDToTask( id );
}

////////////////////////////////////////////////////////////////////////////
// TaskAllocator:
////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////
inline TaskAllocator::~TaskAllocator()
{
	ASSERT( AreAllTasksReleased() );
}

////////////////////////////////////////////////////////////////////////////
inline Task* TaskAllocator::TaskIDToTask( TASK_ID id )
{
	ASSERT( id != INVALID_TASK_ID );
	return &m_taskPool[ id ];
}

////////////////////////////////////////////////////////////////////////////
inline uint32_t TaskAllocator::GetTaskPoolSize()
{
	return TASK_POOL_SIZE;
}

////////////////////////////////////////////////////////////////////////////
inline const TaskRegistry* TaskAllocator::GetTaskRegistryInstance() const
{
	return &m_registry;
}

NAMESPACE_STS_END