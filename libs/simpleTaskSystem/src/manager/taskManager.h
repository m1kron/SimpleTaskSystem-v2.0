#pragma once
#include "..\task\taskAllocator.h"
#include "..\taskWorker\taskWorkersPool.h"
#include "..\task\taskHelpers.h"

#include "..\..\..\basicThreadingLib\include\atomic\atomic.h"
#include "..\..\..\basicThreadingLib\include\thread\thisFiberHelpers.h"

NAMESPACE_STS_BEGIN

class TaskBatch;
class TaskFiber;

class TaskManager
{
public:
	~TaskManager();

	// Starts sts machinary.
	bool Initialize();

	// Stops sts machinery. 
	void Deinitialize();

	// Returns how many workers manager has.
	uint32_t GetWorkersCount() const;

	// Tasks will be processed by workers and this thread until condition is satified. 
	// Function blocks until all needed tasks to satisfy condition are excecuted.
	template< typename TCondition > void RunTasksUsingThisThreadUntil( const TCondition& condition );

	// Creates raw task, which has to be later submitted.
	TaskHandle CreateNewTask( Task::TFunctionPtr task_function, const TaskHandle& parent_task_handle = INVALID_TASK_HANDLE );

	// Creates new functor task.
	template< typename TFunctor > TaskHandle CreateNewTask( const TFunctor& functor, const TaskHandle& parent_task_handle = INVALID_TASK_HANDLE );

	// Submits and dispatches task to workers. Returns false in case of fail.
	bool SubmitTask( const TaskHandle& task_handle );

	// Submits and dispatches whole batch. Returns fail if any of the task failed to be submitted.
	bool SubmitTaskBatch( const TaskBatch& batch );

	// Release task back to the pool. Means that user has finished copying data from task.
	void ReleaseTask( TaskHandle& task_handle );

	// Returns true if all tasks are released.
	bool AreAllTasksReleased() const;

private:
	// Dispatches single task. Returs true if success.
	bool DispatchTask( const TaskHandle& task_handle );

	// Allocates new task and set optional parent.
	TaskHandle CreateNewTaskImpl( const TaskHandle& parent_task_handle = INVALID_TASK_HANDLE );

	// Tries to steal and process one task. Blocking function.
	void TryToRunOneTask();

	// Temporary converts main thread to workers. Needed for running tasks on main thread.
	bool ConvertMainThreadToWorker();

	// Converts from 'worker' back to 'main thread'.
	void ConvertWorkerToMainThread();

	// Wake ups all worker threads;
	void WakeUpAllWorkers() const;

	TaskWorkersPool     m_workerThreadsPool;
	TaskAllocator       m_taskAllocator;
	btl::Atomic< uint32_t >  m_taskDispacherCounter; ///< [NOTE]: does it have to be atomic?
	btl::FIBER_ID		m_thisFiberID;
	TaskFiber*		m_currentTaskFiber;
};

///////////////////////////////////////////////////////////////
//
// INLINES:
//
///////////////////////////////////////////////////////////////
inline uint32_t TaskManager::GetWorkersCount() const
{
	return m_workerThreadsPool.GetPoolSize();
}

///////////////////////////////////////////////////////////////
template< typename TFunctor > 
inline TaskHandle TaskManager::CreateNewTask( const TFunctor& functor, const TaskHandle& parent_task_handle )
{
	TaskHandle new_task_handle = CreateNewTaskImpl( parent_task_handle );

	// Set functor:
	if( new_task_handle != INVALID_TASK_HANDLE )
		FunctorTaskMaker( new_task_handle, functor );

	return new_task_handle;
}

///////////////////////////////////////////////////////////////
template< typename TCondition > 
inline void TaskManager::RunTasksUsingThisThreadUntil( const TCondition& condition )
{
	VERIFY_SUCCES( ConvertMainThreadToWorker(); );

	while( !condition() )
	{
		TryToRunOneTask();
	}

	ConvertWorkerToMainThread();
}

NAMESPACE_STS_END