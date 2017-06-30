#pragma once
#include "taskWorkerInstanceContext.h"
#include "..\structures\lockbased\lockBasedPtrQueue.h"
#include "..\..\..\..\basicThreadingLib\include\thread\thisFiberHelpers.h"
#include "..\..\..\..\basicThreadingLib\include\thread\thisThreadHelpers.h"

NAMESPACE_STS_BEGIN

class Task;
class TaskFiber;

// Class represents TaskWorkerInstance, which is responsible for logic related to running task.
// Implements every thing that is connected to that ( including stealing, fiber managment etc ).
// Note that this class is used by task worker thread, as well as task system to allow running 
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

	// Converts thread to worker instance. Returns true if success.
	bool ConvertToWorkerInstance();

	// Returns true if thread is already converted to worker instance.
	bool IsConvertedToWorkerInstance() const;

	// Converts worker instance back to thread. Returns true if success.
	bool ConvertToNormalThread();

	// Adds task to pending execution queue. Returns true if success.
	bool AddTask( Task* task );

	// Performs one execution step - first checks suspended tasks. Second, tries to execute task from pending queue. If queue is empty, then tries to steal task
	// from other worker instances. Returns true if there is more work to do, false it there is no work on this worker instance.
	bool PerformOneExecutionStep();

	// Returns thread id that this instance is working on.
	btl::THREAD_ID GetThreadID() const;

	// Returns this instance id.
	uint32_t GetInstanceID() const;

	// Returns true if this instance has a work to do ( pending tasks or suspended fibers ).
	bool HasWorkToDo() const;

	// Flushes all pending and suspened tasks by moving them to other worker instances.
	void FlushAllPendingAndSuspendedTasks();

private:
	friend class Dispatcher;

	// Takes ownership of suspended task fiber. Returns true if success.
	bool TakeOwnershipOfSuspendedTaskFiber( TaskFiber* suspended_task_fiber );

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
	bool m_convertedToFiberByThisWorkerInstance;
	btl::Atomic< uint32_t > m_convertedFlag;
	btl::FIBER_ID m_thisFiberID;
	btl::THREAD_ID m_convertedThreadID;
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
	, m_convertedThreadID( INVALID_THREAD_ID )
	, m_convertedToFiberByThisWorkerInstance( false )
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
inline bool TaskWorkerInstance::IsConvertedToWorkerInstance() const
{
	auto val = m_convertedFlag.Load( btl::MemoryOrder::Acquire );
	return val == 1 && m_thisFiberID != INVALID_FIBER_ID;
}

//////////////////////////////////////////////////////////////////////////////////
inline Task* TaskWorkerInstance::TryToStealTaskFromThisInstance()
{
	return m_pendingTaskQueue.PopFront();
}

//////////////////////////////////////////////////////////////////////////////////
inline btl::THREAD_ID TaskWorkerInstance::GetThreadID() const
{
	return m_convertedThreadID;
}

//////////////////////////////////////////////////////////////////////////////////
inline uint32_t TaskWorkerInstance::GetInstanceID() const
{
	return m_context.m_id;
}

//////////////////////////////////////////////////////////////////////////////////
inline bool TaskWorkerInstance::HasWorkToDo() const
{
	return m_suspendedTaskFibers.GetCurrentSize() > 0 || m_pendingTaskQueue.GetCurrentSize() > 0;
}

NAMESPACE_STS_END