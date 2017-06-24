#pragma once
#include "iTaskHandle.h"

namespace sts
{


class ITaskManager
{
public:

	// Returns how many workers manager has.
	virtual int GetWorkersCount() const = 0;

	// Creates raw task, which has to be later submitted. 
	virtual const ITaskHandle* CreateNewTask( const ITaskHandle* parent_task_handle ) = 0;

	// Submits and dispatches task to workers. Returns false in case of fail. Since now task is considered
	// to be executed, so any changes until ITaskHandle::IsFinished() returns true are not permitted.
	virtual bool SubmitTask( const ITaskHandle* task_handle ) = 0;

	// Release task back to the pool. Means that user has finished copying data from task.
	virtual void ReleaseTask( const ITaskHandle* task_handle ) = 0;

	// Tasks will be processed by workers and this thread until condition is satified. 
	// Function blocks until all needed tasks to satisfy condition are excecuted.
	// Returns true if condition is satisfied. Returns false when some critical error occured.
	template< typename TCondition > bool RunTasksUsingThisThreadUntil( const TCondition& condition );

	// Creates a tasks and set task_function at the same time.
	const ITaskHandle* CreateNewTask( sts::TTaskFunctionPtr task_function, const ITaskHandle* parent_task_handle );

protected:
	// Tries to steal and process one task. Blocking function.
	virtual void TryToRunOneTask() = 0;

	// Temporary converts main thread to workers. Needed for running tasks on main thread.
	virtual bool ConvertMainThreadToWorker() = 0;

	// Converts from 'worker' back to 'main thread'.
	virtual void ConvertWorkerToMainThread() = 0;

	virtual ~ITaskManager() = 0;
};

////////////////////////////////////////////////////////////////////////////
//
// INLINES:
//
////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////
inline const ITaskHandle* ITaskManager::CreateNewTask( sts::TTaskFunctionPtr task_function, const ITaskHandle* parent_task_handle )
{
	if( auto task_handle = CreateNewTask( parent_task_handle ) )
	{
		task_handle->SetTaskFunction( task_function );
		return task_handle;
	}

	return nullptr;
}

////////////////////////////////////////////////////////////////////////////
template<typename TCondition>
inline bool ITaskManager::RunTasksUsingThisThreadUntil( const TCondition & condition )
{
	if( !ConvertMainThreadToWorker() )
		return false;

	while( !condition() )
		TryToRunOneTask();

	ConvertWorkerToMainThread();
	return true;
}

}