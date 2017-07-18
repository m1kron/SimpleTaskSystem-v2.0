#pragma once
#include "taskWorkerInstanceContext.h"
#include "..\structures\lockbased\lockBasedPtrQueue.h"
#include "..\..\..\..\basicThreadingLib\include\thread\thisFiberHelpers.h"
#include "..\..\..\..\basicThreadingLib\include\thread\thisThreadHelpers.h"

NAMESPACE_STS_BEGIN

namespace backend
{
	class Task;
	class TaskFiber;
}

NAMESPACE_COMMON_BEGIN

// Class is responsible for task execution and managment. In particular, it
// has a queue of pending tasks, as well as list of suspended ones. It also
// implements fiber managment.
// Note that this class is used by task worker thread, as well as fronted task system 
// in order to allow fronted to act as a worker instance.
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
	bool AddTask( backend::Task* task );

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
	friend class backend::Dispatcher;

	// Takes ownership of suspended task fiber. Returns true if success.
	bool TakeOwnershipOfSuspendedTaskFiber( backend::TaskFiber* suspended_task_fiber );

	// Tries to steal a task from this worker instance.
	backend::Task* TryToStealTaskFromThisInstance();

	// Setups given fiber.
	void SetupFiber( backend::TaskFiber* fiber );

	// Releases fiber.
	void ReleaseFiber( backend::TaskFiber* fiber );

	// Switches to given fiber.
	void SwitchToTaskFiber( backend::TaskFiber* fiber );

	// Handles situation when current task fiber switches back to this worker.
	void HandleCurrentTaskFiberSwitch();

	// Handles task fiber which has finished it's task.
	void OnFinishedTaskFiber( backend::TaskFiber* fiber );

	// Handles current task fiber which has suspended it's execution.
	void OnSuspendedCurrrentTaskFiber();

	// Checks if suspended task fiber can continue to execute now. Returns true there were any suspened 
	// task fibers. This executes until there all suspened fibers are tried or one fiber is done.
	bool CheckAndExecuteSuspenedTaskFibers();

	// Handles switch from suspended fiber task. Returns false if given fiber is still suspended,
	// true when it is done.
	bool HandleSuspendedTaskFiberSwitch( backend::TaskFiber* fiber );

	// Executes single task.
	void ExecuteSingleTask( backend::Task* task );

	// Returns task to execute. Can return nullptr, which means that there is nothing to do.
	backend::Task* TrytoGetTaskToExecute();

	// Tries to steal a task from other worker instances.
	backend::Task* TryToStealTaskFromOtherInstances();

	//---------------------------------------------------------------------

	LockBasedPtrQueue< backend::TaskFiber, TASK_FIBER_POOL_SIZE > m_suspendedTaskFibers;
	LockBasedPtrQueue< backend::Task, TASK_POOL_SIZE > m_pendingTaskQueue;
	TaskWorkerInstanceContext m_context;
	backend::TaskFiber* m_currentFiber;
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
inline backend::Task* TaskWorkerInstance::TryToStealTaskFromThisInstance()
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

NAMESPACE_COMMON_END
NAMESPACE_STS_END