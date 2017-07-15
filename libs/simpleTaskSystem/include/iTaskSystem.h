#pragma once
#include "iTaskHandle.h"

namespace sts
{

// This is the main interface to the system.
class ITaskSystem
{
public:

	// Returns how many workers system has.
	virtual uint32_t GetWorkersCount() const = 0;

	// Creates raw task, which has to be later submitted. 
	virtual const ITaskHandle* CreateNewTask( const ITaskHandle* dependant1, const ITaskHandle* dependant2 = nullptr, const ITaskHandle* dependant3 = nullptr ) = 0;

	// Submits and dispatches task to workers. Returns false in case of fail. Since now task is considered
	// to be executed, so any changes until ITaskHandle::IsFinished() returns true are not permitted.
	virtual bool SubmitTask( const ITaskHandle* task_handle ) = 0;

	// Release task back to the pool. Means that user has finished copying data from task.
	virtual void ReleaseTask( const ITaskHandle* task_handle ) = 0;

	// Function blocks until condition is satisfied. Returns false when some critical error occured.
	template< typename TCondition > bool WaitUntil( const TCondition& condition );

	// Creates a tasks and set task_function at the same time.
	const ITaskHandle* CreateNewTask( sts::TTaskFunctionPtr task_function, const ITaskHandle* dependant1, const ITaskHandle* dependant2 = nullptr, const ITaskHandle* dependant3 = nullptr );

protected:
	// Bunch of functions needed for WaitUntill implementation:
	virtual void WaitOnConvertedMainThread() = 0;
	virtual bool ConvertMainThreadToWorkerInstance() = 0;
	virtual void ConvertWorkerInstanceToMainThread() = 0;
	virtual bool IsOnWorkerInstance() const = 0;
	virtual void WaitOnWorkerInstance() const = 0;
	// ---

	virtual ~ITaskSystem() = 0;
};

////////////////////////////////////////////////////////////////////////////
//
// INLINES:
//
////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////
inline const ITaskHandle* ITaskSystem::CreateNewTask( sts::TTaskFunctionPtr task_function, const ITaskHandle* dependant1, const ITaskHandle* dependant2, const ITaskHandle* dependant3 )
{
	if( auto task_handle = CreateNewTask( dependant1, dependant2, dependant3 ) )
	{
		task_handle->SetTaskFunction( task_function );
		return task_handle;
	}

	return nullptr;
}

////////////////////////////////////////////////////////////////////////////
template<typename TCondition>
inline bool ITaskSystem::WaitUntil( const TCondition& condition )
{
	if( IsOnWorkerInstance() )
	{
		while( !condition() )
			WaitOnWorkerInstance();
	}
	else
	{
		if( !ConvertMainThreadToWorkerInstance() )
			return false;

		while( !condition() )
			WaitOnConvertedMainThread();

		ConvertWorkerInstanceToMainThread();
	}

	return true;
}

}