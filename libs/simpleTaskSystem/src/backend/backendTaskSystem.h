#pragma once
#include "task\taskAllocator.h"
#include "taskWorkerThread\taskWorkerThreadPool.h"
#include "taskFiber\taskFiberAllocator.h"
#include "dispatcher\dispatcher.h"

NAMESPACE_STS_BEGIN

class ITaskSystem;

// Heart of sts system.
class BackendTaskSystem
{
public:
	BackendTaskSystem();
	~BackendTaskSystem();

	// Starts sts machinary.
	bool Initialize( TaskWorkerInstance& helper_instance, ITaskSystem* system_interface );

	// Stops sts machinery. 
	void Deinitialize( TaskWorkerInstance& helper_instance );

	// Creates new tasks.
	const ITaskHandle* CreateNewTask();

	// Releases tasks.
	void ReleaseTask( const ITaskHandle* task_handle );

	// Submit tasks. Returns true if success.
	bool SubmitTask( const ITaskHandle* task_handle );

	// Returns true if all tasks are released.
	bool AreAllTasksReleased() const;

	// Returns how many thread workers system has.
	uint32_t GetWorkersCount() const;

private:
	// Wake ups all worker threads.
	void WakeUpAllWorkerThreads() const;

	TaskWorkersPool			m_workerThreadsPool;
	TaskFiberAllocator		m_taskFiberAllocator;
	TaskAllocator			m_taskAllocator;
	Dispatcher				m_dispatcher;
};

//////////////////////////////////////////////////////////////////
//
// INLINES:
//
//////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////
inline BackendTaskSystem::BackendTaskSystem()
{
}

//////////////////////////////////////////////////////
inline BackendTaskSystem::~BackendTaskSystem()
{
	ASSERT( m_workerThreadsPool.GetPoolSize() == 0 );
	ASSERT( m_taskAllocator.AreAllTasksReleased() );
}

/////////////////////////////////////////////////////////
inline void BackendTaskSystem::ReleaseTask( const ITaskHandle* task_handle )
{
	m_taskAllocator.ReleaseTask( task_handle );
}

/////////////////////////////////////////////////////////
inline bool BackendTaskSystem::AreAllTasksReleased() const
{
	return m_taskAllocator.AreAllTasksReleased();
}

/////////////////////////////////////////////////////////
inline uint32_t BackendTaskSystem::GetWorkersCount() const
{
	return m_workerThreadsPool.GetPoolSize();
}

NAMESPACE_STS_END