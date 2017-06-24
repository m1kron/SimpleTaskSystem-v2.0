#pragma once
#include "taskWorkerInstanceContext.h"
#include "..\structures\lockbased\lockBasedPtrQueue.h"
#include "..\..\..\basicThreadingLib\include\thread\thisFiberHelpers.h"

NAMESPACE_STS_BEGIN

class Task;

// Class represents TaskWorkerInstance, which is responsible for logic related to running task.
// Implements every thing that is connected to that ( including stealing, fiber managment etc ).
// Note that this class is used by task worker thread, as well as task manager to allow running 
// tasks from main thread.
class TaskWorkerInstance
{
public:
	TaskWorkerInstance();
	~TaskWorkerInstance();

	// Initlalizes. Returns true if success.
	bool Initalize( const TaskWorkerInstanceContext& context );

	// Initlalizes. Returns true if success.
	bool Deinitalize();

	// Converts thread to fiber. Returns true if success.
	bool ConvertToFiber();

	// Returns true if thread is already converted to fiber.
	bool IsConvertedToFiber() const;

	// Converts fiber back to thread. Returns true if success.
	bool ConvertToThread();

	// Adds task to pending execution queue. Returns true if success.
	bool AddTask( Task* task );

	// Tries to execute one task - first from pending queue. If queue is empty, then tries to steal task
	// from other worker instances. Returns true if there is more work to do, false it there is no work on this worker instance.
	bool TryToExecuteSingleTask();

private:
	// Tries to steal a task from this worker instance.
	Task* TryToStealTaskFromThisInstance();

	// Setups given fiber.
	void SetupFiber( TaskFiber* fiber );

	// Releases fiber.
	void ReleaseFiber( TaskFiber* fiber );

	// Switches to given fiber.
	void SwitchToTaskFiber( TaskFiber* fiber );

	// Handles situation when current task fiber switches back to this worker.
	void HandleCurrentTaskFiberSwitch();

	// Handles task fiber which has finished it's task.
	void OnFinishedTaskFiber( TaskFiber* fiber );

	// Handles current task fiber which has suspended it's execution.
	void OnSuspendedCurrrentTaskFiber();

	// Checks if suspended task fiber can continue to execute now. Returns true there were any suspened 
	// task fibers. This executes until there all suspened fibers are tried or one fiber is done.
	bool CheckAndExecuteSuspenedTaskFibers();

	// Handles switch from suspended fiber task. Returns false if given fiber is still suspended,
	// true when it is done.
	bool HandleSuspendedTaskFiberSwitch( TaskFiber* fiber );

	// Executes single task.
	void ExecuteSingleTask( Task* task );

	// Returns task to execute. Can return nullptr, which means that there is nothing to do.
	Task* TrytoGetTaskToExecute();

	// Tries to steal a task from other worker instances.
	Task* TryToStealTaskFromOtherInstances();

	//---------------------------------------------------------------------

	LockBasedPtrQueue< TaskFiber, TASK_FIBER_POOL_SIZE > m_suspendedTaskFibers;
	LockBasedPtrQueue< Task, TASK_POOL_SIZE > m_pendingTaskQueue;
	TaskWorkerInstanceContext m_context;
	TaskFiber* m_currentFiber;
	btl::FIBER_ID m_thisFiberID;
};

//////////////////////////////////////////////////////////////////////////////
//
// INLINES:
//
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////
inline TaskWorkerInstance::TaskWorkerInstance()
	: m_currentFiber( nullptr )
	, m_thisFiberID( INVALID_FIBER_ID )
{
}

//////////////////////////////////////////////////////////////////////////////////
inline TaskWorkerInstance::~TaskWorkerInstance()
{
}

//////////////////////////////////////////////////////////////////////////////////
inline bool TaskWorkerInstance::Deinitalize()
{
	ReleaseFiber( m_currentFiber );
	return true;
}

//////////////////////////////////////////////////////////////////////////////////
inline bool TaskWorkerInstance::IsConvertedToFiber() const
{
	return m_thisFiberID != INVALID_FIBER_ID;
}

//////////////////////////////////////////////////////////////////////////////////
inline Task* TaskWorkerInstance::TryToStealTaskFromThisInstance()
{
	return m_pendingTaskQueue.PopFront();
}

NAMESPACE_STS_END