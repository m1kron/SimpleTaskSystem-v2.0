#pragma once
#include "..\..\include\iTaskManager.h"

#include "..\task\taskAllocator.h"
#include "..\taskWorker\taskWorkersPool.h"
#include "..\task\taskHelpers.h"

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
	const ITaskHandle* CreateNewTask( TTaskFunctionPtr task_function, const ITaskHandle* parent_task_handle ) override;
	bool SubmitTask( const ITaskHandle* task_handle ) override;
	//virtual bool SubmitTaskBatch( const TaskBatch& batch ) override;
	void ReleaseTask( const ITaskHandle* task_handle ) override;
	void TryToRunOneTask() override;
	bool ConvertMainThreadToWorker() override;
	void ConvertWorkerToMainThread() override;
	// ---

	// Starts sts machinary.
	bool Initialize();

	// Stops sts machinery. 
	void Deinitialize();

	// Creates new functor task.
	//template< typename TFunctor > TaskHandle CreateNewTask( const TFunctor& functor, const TaskHandle& parent_task_handle = INVALID_TASK_HANDLE );

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
	TaskAllocator			m_taskAllocator;
	btl::Atomic< uint32_t > m_taskDispacherCounter; ///< [NOTE]: does it have to be atomic?
	btl::FIBER_ID			m_thisFiberID;
	TaskFiber*				m_currentTaskFiber;
};

///////////////////////////////////////////////////////////////
//
// INLINES:
//
///////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////
//template< typename TFunctor > 
//inline TaskHandle TaskManager::CreateNewTask( const TFunctor& functor, const TaskHandle& parent_task_handle )
//{
//	TaskHandle new_task_handle = CreateNewTaskImpl( parent_task_handle );
//
//	// Set functor:
//	if( new_task_handle != INVALID_TASK_HANDLE )
//		FunctorTaskMaker( new_task_handle, functor );
//
//	return new_task_handle;
//}

NAMESPACE_STS_END