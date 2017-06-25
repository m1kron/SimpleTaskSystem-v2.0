#pragma once
#include "..\..\include\iTaskManager.h"

#include "..\task\taskAllocator.h"
#include "..\taskWorker\taskWorkersPool.h"
#include "..\taskFiber\taskFiberAllocator.h"
#include "..\backend\dispatcher\dispatcher.h"

NAMESPACE_STS_BEGIN

class TaskBatch;
class TaskFiber;

// Heart of sts system.
// TODO: This has to be splitted into submodules.
class TaskManager : public ITaskManager
{
public:
	~TaskManager();

	// ITaskManager interface:
	int GetWorkersCount() const override;
	const ITaskHandle* CreateNewTask( const ITaskHandle* parent_task_handle ) override;
	bool SubmitTask( const ITaskHandle* task_handle ) override;
	void ReleaseTask( const ITaskHandle* task_handle ) override;
	void TryToRunOneTask() override;
	bool ConvertMainThreadToWorker() override;
	void ConvertWorkerToMainThread() override;
	// ---

	// Starts sts machinary.
	bool Initialize();

	// Stops sts machinery. 
	void Deinitialize();

	// Returns true if all tasks are released.
	bool AreAllTasksReleased() const;

private:
	// Allocates new task and set optional parent.
	const ITaskHandle* CreateNewTaskImpl( const ITaskHandle* parent_task_handle );

	// Wake ups all worker threads;
	void WakeUpAllWorkers() const;

	TaskWorkersPool			m_workerThreadsPool;
	TaskFiberAllocator		m_taskFiberAllocator;
	TaskAllocator			m_taskAllocator;
	Dispatcher				m_dispatcher;
	TaskWorkerInstance		m_mainThreadInstanceWorker;
};

//////////////////////////////////////////////////////////////////
//
// INLINES:
//
//////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////
inline TaskManager::~TaskManager()
{
	ASSERT( m_workerThreadsPool.GetPoolSize() == 0 );
	ASSERT( m_taskAllocator.AreAllTasksReleased() );
}

////////////////////////////////////////////////////////
inline const ITaskHandle* TaskManager::CreateNewTask( const ITaskHandle* parent_task_handle )
{
	return CreateNewTaskImpl( parent_task_handle );
}

/////////////////////////////////////////////////////////
inline void TaskManager::ReleaseTask( const ITaskHandle* task_handle )
{
	m_taskAllocator.ReleaseTask( task_handle );
}

/////////////////////////////////////////////////////////
inline bool TaskManager::AreAllTasksReleased() const
{
	return m_taskAllocator.AreAllTasksReleased();
}

/////////////////////////////////////////////////////////
inline int TaskManager::GetWorkersCount() const
{
	return m_workerThreadsPool.GetPoolSize();
}

NAMESPACE_STS_END