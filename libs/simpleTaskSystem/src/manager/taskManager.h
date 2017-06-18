#pragma once
#include "..\..\include\iTaskManager.h"

#include "..\task\taskAllocator.h"
#include "..\taskWorker\taskWorkersPool.h"
#include "..\taskFiber\taskFiberAllocator.h"

#include "..\..\..\basicThreadingLib\include\thread\thisFiberHelpers.h"

NAMESPACE_STS_BEGIN

class TaskBatch;
class TaskFiber;

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

	// Dispatches single task. Returs true if success.
	bool DispatchTask( Task* task );

private:
	// Allocates new task and set optional parent.
	const ITaskHandle* CreateNewTaskImpl( const ITaskHandle* parent_task_handle );

	// Wake ups all worker threads;
	void WakeUpAllWorkers() const;

	TaskWorkersPool			m_workerThreadsPool;
	TaskFiberAllocator		m_taskFiberAllocator;
	TaskAllocator			m_taskAllocator;
	btl::Atomic< uint32_t > m_taskDispacherCounter; //< [NOTE]: does it have to be atomic?
	btl::FIBER_ID			m_thisFiberID;
	TaskFiber*				m_currentTaskFiber;
};

NAMESPACE_STS_END